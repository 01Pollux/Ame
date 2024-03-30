#include "DeviceImpl.hpp"
#include "FrameManager.hpp"
#include "WindowManager.hpp"
#include "SprivBinding.hpp"

#include <Rhi/DeviceCreateDesc.hpp>
#include "../Nri/Log.hpp"

#ifdef AME_PLATFORM_WINDOWS
#include <d3d12sdklayers.h>
#endif

#include "../NriError.hpp"

namespace Ame::Rhi
{
    Device::Impl::Impl(
        const DeviceCreateDesc& Desc)
    {
        if (!CreateDevice(Desc))
        {
            Log::Rhi().Fatal("Unsupported GPU device");
            return;
        }

        Log::Rhi().Assert(m_Device != nullptr, "Failed to create device");
        Log::Rhi().Assert(m_CommandQueue != nullptr, "Failed to create command queue");

        m_FrameManager.Initialize(*m_NRI.GetCoreInterface(), *m_Device, *m_CommandQueue, Desc.FramesInFlight);
        if (Desc.Window)
        {
            m_WindowManager = std::make_unique<WindowManager>(m_NRI, *m_Device, *m_CommandQueue, Desc);
            RegisterBackbufferState();
        }

        SuppressWarningsIfNeeded(Desc);
    }

    Device::Impl::~Impl()
    {
        ClearResources();
    }

    void Device::Impl::CleanupCheck()
    {
#ifndef AME_DIST
        nri::nriReportLiveObjects();
#endif
    }

    //

    GraphicsAPI Device::Impl::GetGraphicsAPI() const
    {
        auto& Desc = m_NRI.GetCoreInterface()->GetDeviceDesc(*m_Device);
        switch (Desc.graphicsAPI)
        {
        case nri::GraphicsAPI::D3D12:
            return GraphicsAPI::DirectX12;
        case nri::GraphicsAPI::VULKAN:
            return GraphicsAPI::Vulkan;
        default:
            std::unreachable();
            break;
        }
    }

    uint64_t Device::Impl::GetFrameCount() const
    {
        return m_FrameManager.GetFrameCount();
    }

    uint8_t Device::Impl::GetFrameIndex() const
    {
        return m_FrameManager.GetFrameIndex();
    }

    uint8_t Device::Impl::GetFrameCountInFlight() const
    {
        return m_FrameManager.GetFrameCountInFlight();
    }

    //

    const Math::Color4& Device::Impl::GetClearColor() const noexcept
    {
        return m_ClearColor;
    }

    void Device::Impl::SetClearColor(
        const Math::Color4& Color)
    {
        m_ClearColor = Color;
    }

    BackbufferClearType Device::Impl::GetBackbufferClearType() const noexcept
    {
        return m_ClearType;
    }

    void Device::Impl::SetBackbufferClearType(
        BackbufferClearType Type)
    {
        m_ClearType = Type;
    }

    //

    bool Device::Impl::IsHeadless() const
    {
        return !m_WindowManager;
    }

    Windowing::Window& Device::Impl::GetWindow() const
    {
        return *m_WindowManager->GetWindow();
    }

    uint8_t Device::Impl::GetBackbufferCount() const
    {
        return m_WindowManager->GetBackbufferCount();
    }

    uint8_t Device::Impl::GetBackbufferIndex() const
    {
        return m_WindowManager->GetBackbufferIndex();
    }

    Backbuffer Device::Impl::GetBackbuffer(
        uint8_t Index) const
    {
        return m_WindowManager->GetBackbuffer(Index);
    }

    Backbuffer Device::Impl::GetBackbuffer() const
    {
        return GetBackbuffer(GetBackbufferIndex());
    }

    //

    bool Device::Impl::IsVSyncEnabled() const noexcept
    {
        return IsHeadless() ? false : m_WindowManager->IsVSyncEnabled();
    }

    void Device::Impl::SetVSyncEnabled(
        bool State)
    {
        if (!IsHeadless())
        {
            m_WindowManager->SetVSyncEnabled(State);
        }
    }

    //

    bool Device::Impl::ProcessEvents()
    {
        if (!IsHeadless()) [[likely]]
        {
            auto& Window = this->GetWindow();
            if (!Window.IsRunning())
            {
                return false;
            }
            Window.ProcessEvents();
        }

        return true;
    }

    void Device::Impl::BeginFrame()
    {
        if (!IsHeadless()) [[likely]]
        {
            if (m_WindowManager->IsDirty()) [[likely]]
            {
                WaitIdle();
                UnregisterBackbufferState();
                m_WindowManager->RecreateSwapchain(*m_Device, *m_CommandQueue);
                RegisterBackbufferState();
            }
        }

        m_FrameManager.NewFrame(*m_NRI.GetCoreInterface());

        if (!IsHeadless()) [[likely]]
        {
            m_WindowManager->NewFrame();
            TransitionBackbuffer(false);
            if (m_ClearType != BackbufferClearType::None)
            {
                ClearBackbuffer(m_ClearColor);
            }
        }
    }

    void Device::Impl::EndFrame()
    {
        auto NriCore = m_NRI.GetCoreInterface();

        if (!IsHeadless()) [[likely]]
        {
            TransitionBackbuffer(true);
        }

        m_FrameManager.EndFrame(*NriCore, *m_CommandQueue);

        if (!IsHeadless()) [[likely]]
        {
            m_WindowManager->Present();
        }

        m_FrameManager.AdvanceFrame(*NriCore, *m_CommandQueue);
    }

    //

    void Device::Impl::WaitIdle()
    {
        m_NRI.WaitIdle(*m_CommandQueue);
        m_FrameManager.FlushIdle();
    }

    //

    NRIBridge& Device::Impl::GetNRI() noexcept
    {
        return m_NRI;
    }

    nri::Device& Device::Impl::GetDevice() noexcept
    {
        return *m_Device;
    }

    nri::CommandBuffer& Device::Impl::GetCurrentCommandList() const noexcept
    {
        return m_FrameManager.GetCurrentCommandList();
    }

    //

    void Device::Impl::RegisterBackbufferState()
    {
        auto NriCore = m_NRI.GetCoreInterface();

        nri::AccessLayoutStage State{ nri::AccessBits::UNKNOWN, nri::Layout::UNKNOWN, nri::StageBits::ALL };
        for (uint32_t i = 0; i < GetBackbufferCount(); i++)
        {
            m_ResourceStateTracker.BeginTracking(*NriCore, GetBackbuffer(i).Resource.Unwrap(), State);
        }
    }

    void Device::Impl::UnregisterBackbufferState()
    {
        auto NriCore = m_NRI.GetCoreInterface();
        for (uint32_t i = 0; i < GetBackbufferCount(); i++)
        {
            m_ResourceStateTracker.EndTracking(GetBackbuffer(i).Resource.Unwrap());
        }
    }

    void Device::Impl::TransitionBackbuffer(
        bool Present)
    {
        auto  NriCore        = m_NRI.GetCoreInterface();
        auto& CurCommandList = GetCurrentCommandList();
        auto  CurBackbuffer  = GetBackbuffer();

        nri::AccessLayoutStage State{ .stages = nri::StageBits::ALL };
        if (Present)
        {
            State.access = nri::AccessBits::UNKNOWN;
            State.layout = nri::Layout::PRESENT;
        }
        else
        {
            State.access = nri::AccessBits::COLOR_ATTACHMENT;
            State.layout = nri::Layout::COLOR_ATTACHMENT;
        }

        m_ResourceStateTracker.RequireState(
            *NriCore,
            CurBackbuffer.Resource.Unwrap(),
            State);

        m_ResourceStateTracker.CommitBarriers(*NriCore, CurCommandList);
    }

    void Device::Impl::ClearBackbuffer(
        const Math::Color4& Color)
    {
        auto  NriCore           = m_NRI.GetCoreInterface();
        auto& CurCommandList    = GetCurrentCommandList();
        auto  CurBackbuffer     = GetBackbuffer();
        auto  CurBackbufferView = CurBackbuffer.View.Unwrap();

        nri::ClearDesc Clears{
            .value{ .color32f{ m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a } },
            .attachmentContentType = nri::AttachmentContentType::COLOR
        };

        nri::AttachmentsDesc Attachments{
            .colors   = &CurBackbufferView,
            .colorNum = 1
        };

        // TODO: CommandList aware of last rendering pass
        NriCore->CmdBeginRendering(CurCommandList, Attachments);
        NriCore->CmdClearAttachments(CurCommandList, &Clears, 1, nullptr, 0);
        NriCore->CmdEndRendering(CurCommandList);
    }

    //

    bool Device::Impl::CreateDevice(
        const DeviceCreateDesc& Desc)
    {
        nri::DeviceCreationDesc NriDeviceDesc{
            .adapterDesc = &Desc.Adapter,
            .callbackInterface{
                .MessageCallback = NriLogCallbackInterface::MessageCallback,
                .AbortExecution  = NriLogCallbackInterface::AbortExecution },
            .spirvBindingOffsets = DefaultSpirvBindingOffset,
            .vulkanExtensions{
                .instanceExtensions   = Desc.RequiredInstanceExtensions.data(),
                .instanceExtensionNum = static_cast<uint32_t>(Desc.RequiredInstanceExtensions.size()),
                .deviceExtensions     = Desc.RequiredDeviceExtensions.data(),
                .deviceExtensionNum   = static_cast<uint32_t>(Desc.RequiredDeviceExtensions.size()) },
            .enableNRIValidation     = Desc.EnableApiValidationLayer,
            .enableAPIValidation     = Desc.EnableApiValidationLayer,
            .disableVulkanRayTracing = Desc.RayTracingFeatures == DeviceFeatureType::Disabled
        };

        auto SwapchainFeatures = Desc.Window.has_value() ? DeviceFeatureType::Required : DeviceFeatureType::Disabled;

        for (int i = static_cast<int>(DeviceType::Auto) + 1; i < static_cast<int>(DeviceType::Count); i++)
        {
            auto Type = Desc.Type;
            if (Type == DeviceType::Auto)
            {
                Type = static_cast<DeviceType>(i);
            }

            switch (Type)
            {
#ifdef AME_RENDERER_BUILD_DX12
            case DeviceType::DirectX12:
                NriDeviceDesc.graphicsAPI = nri::GraphicsAPI::D3D12;
                break;
#endif
#ifdef AME_RENDERER_BUILD_VULKAN
            case DeviceType::Vulkan:
                NriDeviceDesc.graphicsAPI = nri::GraphicsAPI::VULKAN;
                break;
#endif
            default:
                continue;
            }

            if (nri::nriCreateDevice(NriDeviceDesc, m_Device) == nri::Result::SUCCESS &&
                m_NRI.Initialize(*m_Device, SwapchainFeatures, Desc.MeshShaderFeatures, Desc.RayTracingFeatures) &&
                m_NRI.GetCoreInterface()->GetCommandQueue(*m_Device, nri::CommandQueueType::GRAPHICS, m_CommandQueue) == nri::Result::SUCCESS)
            {
                break;
            }

            ClearResources();
        }

        return m_Device != nullptr;
    }

    void Device::Impl::ClearResources()
    {
        if (!m_Device) [[unlikely]]
        {
            return;
        }

        WaitIdle();

        if (m_WindowManager)
        {
            UnregisterBackbufferState();
            m_WindowManager.reset();
        }
        if (auto NriCore = m_NRI.GetCoreInterface())
        {
            m_FrameManager.Shutdown(*NriCore);
        }
        m_NRI.Shutdown();

#ifndef AME_DIST
        nri::nriDestroyDevice(*m_Device);
#endif
        m_Device = nullptr;
    }

    //

    void Device::Impl::SuppressWarningsIfNeeded(
        const DeviceCreateDesc& Desc)
    {
#ifndef AME_DIST
        auto GraphicsAPI = GetGraphicsAPI();
        if (Desc.EnableApiValidationLayer && GraphicsAPI == GraphicsAPI::DirectX12) [[likely]]
        {
#ifdef AME_PLATFORM_WINDOWS
            auto CoreInterface = m_NRI.GetCoreInterface();
            auto Device        = static_cast<ID3D12Device*>(CoreInterface->GetDeviceNativeObject(*m_Device));

            ID3D12InfoQueue* InfoQueue = nullptr;
            Device->QueryInterface(&InfoQueue);

            if (InfoQueue)
            {
                D3D12_MESSAGE_ID DisableMessageIDs[] = {
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
                };

                D3D12_INFO_QUEUE_FILTER Filter{
                    .DenyList{
                        .NumIDs  = 1,
                        .pIDList = DisableMessageIDs }
                };

                InfoQueue->AddStorageFilterEntries(&Filter);
                InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
                InfoQueue->Release();
            }
        }
#endif
#endif
    }
} // namespace Ame::Rhi