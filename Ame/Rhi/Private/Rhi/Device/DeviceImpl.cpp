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
        const DeviceCreateDesc& desc)
    {
        if (!CreateDevice(desc))
        {
            Log::Rhi().Fatal("Unsupported GPU device");
            return;
        }

        Log::Rhi().Assert(m_Device != nullptr, "Failed to create device");
        Log::Rhi().Assert(m_CommandQueue != nullptr, "Failed to create command queue");

        m_MemoryAllocator.Initialize(*this, desc.MemoryAllocator);
        m_FrameManager.Initialize(*this, desc.DescriptorPoolDesc, desc.FramesInFlight);
        if (desc.Window)
        {
            m_WindowManager = std::make_unique<WindowManager>(*this, desc);
            RegisterBackbufferState();
        }

        SuppressWarningsIfNeeded(desc);
        EnableValidationIfNeeded(desc);

        m_DrawIndexedCommandSize = GetDesc().isDrawParametersEmulationEnabled ? sizeof(Rhi::DrawIndexedBaseDesc) : sizeof(Rhi::DrawIndexedDesc);

        m_ResourceStateTracker.Initialize(&GetDesc());
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
        auto& nriCore = *m_NRI.GetCoreInterface();
        auto& desc    = nriCore.GetDeviceDesc(*m_Device);
        switch (desc.graphicsAPI)
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

    const DeviceDesc& DeviceImpl::GetDesc() const
    {
        auto& nriCore = *m_NRI.GetCoreInterface();
        return nriCore.GetDeviceDesc(*m_Device);
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

    uint32_t DeviceImpl::GetDrawIndexedCommandSize() const
    {
        return m_DrawIndexedCommandSize;
    }

    //

    const Math::Color4& DeviceImpl::GetClearColor() const noexcept
    {
        return m_ClearColor;
    }

    void DeviceImpl::SetClearColor(
        const Math::Color4& color)
    {
        m_ClearColor = color;
    }

    BackbufferClearType DeviceImpl::GetBackbufferClearType() const noexcept
    {
        return m_ClearType;
    }

    void DeviceImpl::SetBackbufferClearType(
        BackbufferClearType type)
    {
        m_ClearType = type;
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

    const Backbuffer& DeviceImpl::GetBackbuffer(
        uint8_t index) const
    {
        return m_WindowManager->GetBackbuffer(index);
    }

    const Backbuffer& DeviceImpl::GetBackbuffer() const
    {
        return GetBackbuffer(GetBackbufferIndex());
    }

    //

    bool DeviceImpl::IsVSyncEnabled() const noexcept
    {
        return IsHeadless() ? false : m_WindowManager->IsVSyncEnabled();
    }

    void DeviceImpl::SetVSyncEnabled(
        bool state)
    {
        if (!IsHeadless())
        {
            m_WindowManager->SetVSyncEnabled(state);
        }
    }

    void DeviceImpl::CleanupCache()
    {
        m_PipelineLayoutCache.Clear();
        m_GraphicsPipelineCache.Clear();
        m_ComputePipelineCache.Clear();
    }

    //

    bool DeviceImpl::ProcessEvents() const
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
                m_WindowManager->RecreateSwapchain();
                RegisterBackbufferState();
            }
        }

        auto& nriCore = *m_NRI.GetCoreInterface();
        m_FrameManager.NewFrame(nriCore, m_MemoryAllocator);

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
        auto& nriCore = *m_NRI.GetCoreInterface();

        if (!IsHeadless()) [[likely]]
        {
            TransitionBackbuffer(true);
        }

        m_FrameManager.EndFrame();

        if (!IsHeadless()) [[likely]]
        {
            m_WindowManager->Present();
        }

        m_FrameManager.AdvanceFrame(nriCore, *m_CommandQueue);
    }

    //

    void DeviceImpl::WaitIdle()
    {
        auto& nriCore = *m_NRI.GetCoreInterface();

        m_NRI.WaitIdle(*m_CommandQueue);
        m_FrameManager.FlushIdle(nriCore, m_MemoryAllocator);
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

    nri::CommandQueue& DeviceImpl::GetQueue() noexcept
    {
        return *m_CommandQueue;
    }

    CommandListImpl& DeviceImpl::GetCurrentCommandList() noexcept
    {
        return m_FrameManager.GetCurrentCommandList();
    }

    ResourceStateTracker& DeviceImpl::GetStateTracker() noexcept
    {
        return m_ResourceStateTracker;
    }

    //

    void DeviceImpl::RegisterBackbufferState()
    {
        auto& nriCore = *m_NRI.GetCoreInterface();

        nri::AccessLayoutStage state{ nri::AccessBits::UNKNOWN, nri::Layout::UNKNOWN, nri::StageBits::ALL };
        for (uint32_t i = 0; i < GetBackbufferCount(); i++)
        {
            m_ResourceStateTracker.BeginTracking(nriCore, GetBackbuffer(i).Resource.Unwrap(), state);
        }
    }

    void DeviceImpl::UnregisterBackbufferState()
    {
        for (uint32_t i = 0; i < GetBackbufferCount(); i++)
        {
            m_ResourceStateTracker.EndTracking(GetBackbuffer(i).Resource.Unwrap());
        }
    }

    void DeviceImpl::TransitionBackbuffer(
        bool Present)
    {
        auto& nriCore     = *m_NRI.GetCoreInterface();
        auto& commandList = GetCurrentCommandList();
        auto& backBuffer  = GetBackbuffer();

        Rhi::AccessLayoutStage state{
            .stages = Rhi::ShaderType::ALL
        };
        if (Present)
        {
            state.access = Rhi::AccessBits::UNKNOWN;
            state.layout = Rhi::LayoutType::PRESENT;
        }
        else
        {
            state.access = Rhi::AccessBits::COLOR_ATTACHMENT;
            state.layout = Rhi::LayoutType::COLOR_ATTACHMENT;
        }

        commandList.RequireState(backBuffer.Resource.Unwrap(), state);
        commandList.CommitBarriers();
    }

    void DeviceImpl::ClearBackbuffer(
        const Math::Color4& color)
    {
        auto& nriCore           = *m_NRI.GetCoreInterface();
        auto& commandBuffer     = GetCurrentCommandList().GetCommandBuffer();
        auto& backBuffer        = GetBackbuffer();
        auto  nriBackBufferView = backBuffer.View.Unwrap();

        nri::ClearDesc clears[]{
            { .value{ .color32f{ m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a } },
              .attachmentContentType = nri::AttachmentContentType::COLOR }
        };

        nri::AttachmentsDesc Attachments{
            .colors   = &nriBackBufferView,
            .colorNum = 1
        };

        // TODO: CommandList aware of last rendering pass
        nriCore.CmdBeginRendering(commandBuffer, Attachments);
        nriCore.CmdClearAttachments(commandBuffer, clears, 1, nullptr, 0);
        nriCore.CmdEndRendering(commandBuffer);
    }

    //

    bool DeviceImpl::CreateDevice(
        const DeviceCreateDesc& desc)
    {
        nri::DeviceCreationDesc nriCreateDesc{
            .adapterDesc = &desc.Adapter,
            .callbackInterface{
                .MessageCallback = NriLogCallbackInterface::MessageCallback,
                .AbortExecution  = NriLogCallbackInterface::AbortExecution },
            .memoryAllocatorInterface{
                .Allocate   = NriAllocatorCallbackInterface::Allocate,
                .Reallocate = NriAllocatorCallbackInterface::Reallocate,
                .Free       = NriAllocatorCallbackInterface::Free },
            .spirvBindingOffsets = c_DefaultSpirvBindingOffset,
            .vulkanExtensions{
                .instanceExtensions   = desc.RequiredInstanceExtensions.data(),
                .instanceExtensionNum = static_cast<uint32_t>(desc.RequiredInstanceExtensions.size()),
                .deviceExtensions     = desc.RequiredDeviceExtensions.data(),
                .deviceExtensionNum   = static_cast<uint32_t>(desc.RequiredDeviceExtensions.size()) },
            .enableNRIValidation                = desc.EnableApiValidationLayer,
            .enableAPIValidation                = desc.EnableApiValidationLayer,
            .enableD3D12DrawParametersEmulation = Rhi::Device::EnableDrawParametersEmulation,
            .disableVulkanRayTracing            = desc.RayTracingFeatures == DeviceFeatureType::Disabled
        };

        auto swapchainFeatures = desc.Window.has_value() ? DeviceFeatureType::Required : DeviceFeatureType::Disabled;

        for (int i = static_cast<int>(DeviceType::Auto) + 1; i < static_cast<int>(DeviceType::Count); i++)
        {
            auto deviceType = desc.Type;
            if (deviceType == DeviceType::Auto)
            {
                deviceType = static_cast<DeviceType>(i);
            }

            switch (deviceType)
            {
            case DeviceType::DirectX12:
                nriCreateDesc.graphicsAPI = nri::GraphicsAPI::D3D12;
                break;
            case DeviceType::Vulkan:
                nriCreateDesc.graphicsAPI = nri::GraphicsAPI::VULKAN;
                break;
            default:
                continue;
            }

            if (nri::nriCreateDevice(nriCreateDesc, m_Device) == nri::Result::SUCCESS &&
                m_NRI.Initialize(*m_Device, swapchainFeatures, desc.MeshShaderFeatures, desc.RayTracingFeatures) &&
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

        m_MemoryAllocator.Shutdown();

        if (auto nriCore = m_NRI.GetCoreInterface())
        {
            m_FrameManager.Shutdown(*nriCore);
        }

        m_NRI.Shutdown();

#ifndef AME_DIST
        nri::nriDestroyDevice(*m_Device);
#endif
        m_Device = nullptr;
    }

    //

    void DeviceImpl::SuppressWarningsIfNeeded(
        const DeviceCreateDesc& desc)
    {
#ifndef AME_DIST
        auto GraphicsAPI = GetGraphicsAPI();
        if (desc.EnableApiValidationLayer && GraphicsAPI == GraphicsAPI::DirectX12) [[likely]]
        {
#ifdef AME_PLATFORM_WINDOWS
            auto& nriCore     = *m_NRI.GetCoreInterface();
            auto  d3d12Device = static_cast<ID3D12Device*>(nriCore.GetDeviceNativeObject(*m_Device));

            ID3D12InfoQueue* d3d12InfoQueue = nullptr;
            d3d12Device->QueryInterface(&d3d12InfoQueue);

            if (d3d12InfoQueue)
            {
                D3D12_MESSAGE_ID disableMessageIDs[] = {
                    D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
                };

                D3D12_INFO_QUEUE_FILTER filter{
                    .DenyList{
                        .NumIDs  = 1,
                        .pIDList = disableMessageIDs }
                };

                d3d12InfoQueue->AddStorageFilterEntries(&filter);
                d3d12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
                d3d12InfoQueue->Release();
            }
#endif
        }
#endif
    }

    void DeviceImpl::EnableValidationIfNeeded(
        const DeviceCreateDesc& desc)
    {
#ifndef AME_DIST
        auto Api = GetGraphicsAPI();
        if (desc.EnableApiValidationLayer && Api == GraphicsAPI::DirectX12) [[likely]]
        {
#ifdef AME_PLATFORM_WINDOWS
            auto& nriCore     = *m_NRI.GetCoreInterface();
            auto  d3d12Device = static_cast<ID3D12Device*>(nriCore.GetDeviceNativeObject(*m_Device));

            ID3D12Debug* d3d12Debug = nullptr;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12Debug))))
            {
                d3d12Debug->EnableDebugLayer();
                d3d12Debug->Release();
            }
#endif
        }
#endif
    }
} // namespace Ame::Rhi