#include <Rhi/Device/DeviceImpl.hpp>
#include <Rhi/Device/FrameManager.hpp>
#include <Rhi/Device/WindowManager.hpp>
#include <Rhi/Device/SprivBinding.hpp>

#include <Rhi/Device/CreateDesc.hpp>
#include <Rhi/Nri/Log.hpp>
#include <Rhi/Nri/Allocator.hpp>

#ifdef AME_PLATFORM_WINDOWS
#include <d3d12sdklayers.h>
#endif

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    DeviceImpl::DeviceImpl(
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

    DeviceImpl::~DeviceImpl()
    {
        ClearResources();
    }

    void DeviceImpl::CleanupCheck()
    {
#ifndef AME_DIST
        nri::nriReportLiveObjects();
#endif
    }

    //

    GraphicsAPI DeviceImpl::GetGraphicsAPI() const
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

    uint64_t DeviceImpl::GetFrameCount() const
    {
        return m_FrameManager.GetFrameCount();
    }

    uint8_t DeviceImpl::GetFrameIndex() const
    {
        return m_FrameManager.GetFrameIndex();
    }

    uint8_t DeviceImpl::GetFrameCountInFlight() const
    {
        return m_FrameManager.GetFrameCountInFlight();
    }

    //

    const Math::Color4& DeviceImpl::GetClearColor() const noexcept
    {
        return m_ClearColor;
    }

    void DeviceImpl::SetClearColor(
        const Math::Color4& Color)
    {
        m_ClearColor = Color;
    }

    BackbufferClearType DeviceImpl::GetBackbufferClearType() const noexcept
    {
        return m_ClearType;
    }

    void DeviceImpl::SetBackbufferClearType(
        BackbufferClearType Type)
    {
        m_ClearType = Type;
    }

    //

    bool DeviceImpl::IsHeadless() const
    {
        return !m_WindowManager;
    }

    Windowing::Window& DeviceImpl::GetWindow() const
    {
        return *m_WindowManager->GetWindow();
    }

    uint8_t DeviceImpl::GetBackbufferCount() const
    {
        return m_WindowManager->GetBackbufferCount();
    }

    uint8_t DeviceImpl::GetBackbufferIndex() const
    {
        return m_WindowManager->GetBackbufferIndex();
    }

    Backbuffer DeviceImpl::GetBackbuffer(
        uint8_t Index) const
    {
        return m_WindowManager->GetBackbuffer(Index);
    }

    Backbuffer DeviceImpl::GetBackbuffer() const
    {
        return GetBackbuffer(GetBackbufferIndex());
    }

    //

    bool DeviceImpl::IsVSyncEnabled() const noexcept
    {
        return IsHeadless() ? false : m_WindowManager->IsVSyncEnabled();
    }

    void DeviceImpl::SetVSyncEnabled(
        bool State)
    {
        if (!IsHeadless())
        {
            m_WindowManager->SetVSyncEnabled(State);
        }
    }

    //

    bool DeviceImpl::ProcessEvents()
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

    void DeviceImpl::BeginFrame()
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

    void DeviceImpl::EndFrame()
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

    void DeviceImpl::WaitIdle()
    {
        auto NriCore = m_NRI.GetCoreInterface();

        m_NRI.WaitIdle(*m_CommandQueue);
        m_FrameManager.FlushIdle(*NriCore);
    }

    //

    NRIBridge& DeviceImpl::GetNRI() noexcept
    {
        return m_NRI;
    }

    nri::Device& DeviceImpl::GetDevice() noexcept
    {
        return *m_Device;
    }

    CommandListImpl& DeviceImpl::GetCurrentCommandList() noexcept
    {
        return m_FrameManager.GetCurrentCommandList();
    }

    //

    void DeviceImpl::RegisterBackbufferState()
    {
        auto NriCore = m_NRI.GetCoreInterface();

        nri::AccessLayoutStage State{ nri::AccessBits::UNKNOWN, nri::Layout::UNKNOWN, nri::StageBits::ALL };
        for (uint32_t i = 0; i < GetBackbufferCount(); i++)
        {
            m_ResourceStateTracker.BeginTracking(*NriCore, GetBackbuffer(i).Resource.Unwrap(), State);
        }
    }

    void DeviceImpl::UnregisterBackbufferState()
    {
        auto NriCore = m_NRI.GetCoreInterface();
        for (uint32_t i = 0; i < GetBackbufferCount(); i++)
        {
            m_ResourceStateTracker.EndTracking(GetBackbuffer(i).Resource.Unwrap());
        }
    }

    void DeviceImpl::TransitionBackbuffer(
        bool Present)
    {
        auto  NriCore        = m_NRI.GetCoreInterface();
        auto& CommandBuffer  = GetCurrentCommandList().GetCommandBuffer();
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

        m_ResourceStateTracker.CommitBarriers(*NriCore, CommandBuffer);
    }

    void DeviceImpl::ClearBackbuffer(
        const Math::Color4& Color)
    {
        auto  NriCore           = m_NRI.GetCoreInterface();
        auto& CommandBuffer    = GetCurrentCommandList().GetCommandBuffer();
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
        NriCore->CmdBeginRendering(CommandBuffer, Attachments);
        NriCore->CmdClearAttachments(CommandBuffer, &Clears, 1, nullptr, 0);
        NriCore->CmdEndRendering(CommandBuffer);
    }

    //

    bool DeviceImpl::CreateDevice(
        const DeviceCreateDesc& Desc)
    {
        nri::DeviceCreationDesc NriDeviceDesc{
            .adapterDesc = &Desc.Adapter,
            .callbackInterface{
                .MessageCallback = NriLogCallbackInterface::MessageCallback,
                .AbortExecution  = NriLogCallbackInterface::AbortExecution },
            .memoryAllocatorInterface{
                .Allocate   = NriAllocatorCallbackInterface::Allocate,
                .Reallocate = NriAllocatorCallbackInterface::Reallocate,
                .Free       = NriAllocatorCallbackInterface::Free },
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
            case DeviceType::DirectX12:
                NriDeviceDesc.graphicsAPI = nri::GraphicsAPI::D3D12;
                break;
            case DeviceType::Vulkan:
                NriDeviceDesc.graphicsAPI = nri::GraphicsAPI::VULKAN;
                break;
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

    void DeviceImpl::ClearResources()
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

    void DeviceImpl::SuppressWarningsIfNeeded(
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
#endif
        }
#endif
    }
} // namespace Ame::Rhi