#include <Rhi/Device/Device/Device.Impl.hpp>
#include <Rhi/Device/SprivBinding.hpp>

#include <Rhi/Device/CreateDesc.hpp>
#include <Rhi/Device/Swapchain/WindowManager.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    static std::atomic_uint32_t s_DeviceCount = 0;

    //

    DeviceImpl::DeviceImpl(
        Co::runtime&            runtime,
        const DeviceCreateDesc& desc) :
        m_ResourceAllocator(*this, desc.MemoryDesc.MultiThreaded),
        m_CommandSubmitter(*this),
        m_DeviceWindowManager(*this),
        m_ConcurrentLoopCount(desc.ConcurrentLoopCount),
        m_ConcurrentLoopChunkSize(desc.ConcurrentLoopChunkSize)
    {
        s_DeviceCount.fetch_add(1);

        if (!CreateDevice(desc))
        {
            Log::Rhi().Fatal("Unsupported GPU device");
            return;
        }

        for (auto& executor : m_Executors)
        {
            executor = runtime.make_manual_executor();
        }

        if (desc.Window)
        {
            m_WindowManager = std::make_unique<WindowManager>(m_ResourceAllocator, desc);
        }

        m_FrameManager.Initialize(*m_DeviceWrapper, desc.FramesInFlight);

        m_DrawIndexedCommandSize = GetDesc().isDrawParametersEmulationEnabled ? sizeof(Rhi::DrawIndexedBaseDesc) : sizeof(Rhi::DrawIndexedDesc);
    }

    DeviceImpl::~DeviceImpl()
    {
        ClearResources();

        if (s_DeviceCount.fetch_sub(1) == 1)
        {
            nri::nriReportLiveObjects();
        }
    }

    //

    const Ptr<Co::manual_executor>& DeviceImpl::GetExecutor(
        ExecutorType type) const
    {
        return m_Executors[static_cast<size_t>(type)];
    }

    IDeviceWrapper& DeviceImpl::GetDeviceWrapper() const noexcept
    {
        return *m_DeviceWrapper;
    }

    FrameManager& DeviceImpl::GetFrameManager() noexcept
    {
        return m_FrameManager;
    }

    WindowManager* DeviceImpl::GetWindowManager() noexcept
    {
        return m_WindowManager.get();
    }

    CommandListManager& DeviceImpl::GetCommandListManager() noexcept
    {
        return m_CommandListManager;
    }

    //

    GraphicsAPI DeviceImpl::GetGraphicsAPI() const
    {
        auto& deviceDesc = GetDesc();
        switch (deviceDesc.graphicsAPI)
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
        auto& nri     = m_DeviceWrapper->GetNri();
        auto& nriCore = *nri.GetCoreInterface();
        return nriCore.GetDeviceDesc(m_DeviceWrapper->GetNriDevice());
    }

    uint32_t DeviceImpl::GetDrawIndexedCommandSize() const
    {
        return m_DrawIndexedCommandSize;
    }

    //

    bool DeviceImpl::BeginFrame()
    {
        AME_LOG_ASSERT(Log::Rhi(), s_RenderingThreadId == std::thread::id{}, "The rendering thread is already running");

        if (!ProcessEvents())
        {
            return false;
        }

        s_RenderingThreadId = std::this_thread::get_id();
        StartFrame();
        return true;
    }

    void DeviceImpl::ProcessTasks()
    {
        FrameUpdate();
    }

    void DeviceImpl::EndFrame()
    {
        AME_LOG_ASSERT(Log::Rhi(), s_RenderingThreadId == std::this_thread::get_id(), "The rendering thread is not running");

        FrameEnd();
        s_RenderingThreadId = std::thread::id{};
    }

    Co::result<void> DeviceImpl::WaitIdle()
    {
        AssertInRenderingThread(false);

        auto& memoryAllocator = m_DeviceWrapper->GetMemoryAllocator();
        auto& nri             = m_DeviceWrapper->GetNri();
        auto& nriCore         = *nri.GetCoreInterface();

        for (auto& commandQueue : m_DeviceWrapper->GetQueueSet())
        {
            nri.WaitIdle(*commandQueue);
        }

        m_FrameManager.FlushIdle(nriCore, memoryAllocator);
        co_return;
    }

    //

    DeviceResourceAllocator& DeviceImpl::GetResourceAllocator()
    {
        return m_ResourceAllocator;
    }

    DeviceCommandSubmitter& DeviceImpl::GetCommandSubmitter()
    {
        return m_CommandSubmitter;
    }

    DeviceWindowManager& DeviceImpl::GetDeviceWindowManager()
    {
        return m_DeviceWindowManager;
    }

    //

    bool DeviceImpl::CreateDevice(
        const DeviceCreateDesc& desc)
    {
        for (int i = static_cast<int>(DeviceType::Auto) + 1; i < static_cast<int>(DeviceType::Count); i++)
        {
            auto deviceType = desc.Type;
            if (deviceType == DeviceType::Auto)
            {
                deviceType = static_cast<DeviceType>(i);
            }

            m_DeviceWrapper = IDeviceWrapper::Create(desc, deviceType);
            if (m_DeviceWrapper)
            {
                break;
            }

            ClearResources();
        }

        return m_DeviceWrapper != nullptr;
    }

    void DeviceImpl::ClearResources()
    {
        if (!m_DeviceWrapper) [[unlikely]]
        {
            return;
        }

        WaitIdle().get();

        if (m_WindowManager)
        {
            m_WindowManager.reset();
        }

        auto& nri     = m_DeviceWrapper->GetNri();
        auto& nriCore = *nri.GetCoreInterface();
        m_FrameManager.Shutdown(nriCore);

        m_CommandListManager.Shutdown(nriCore);

        m_DeviceWrapper = nullptr;
    }

    //

    bool DeviceImpl::ProcessEvents()
    {
        if (m_WindowManager) [[likely]]
        {
            auto& window = *m_WindowManager->GetWindow();
            if (!window.IsRunning())
            {
                return false;
            }
            window.ProcessEvents();

            if (m_WindowManager->IsDirty()) [[unlikely]]
            {
                WaitIdle().wait();
                m_WindowManager->RecreateSwapchain();
            }
        }
        return true;
    }

    void DeviceImpl::StartFrame()
    {
        auto& nri             = m_DeviceWrapper->GetNri();
        auto& memoryAllocator = m_DeviceWrapper->GetMemoryAllocator();
        auto& nriCore         = *nri.GetCoreInterface();

        m_FrameManager.NewFrame(nriCore, memoryAllocator);
        auto& currentFrame = m_FrameManager.GetCurrentFrame();

        if (m_WindowManager) [[likely]]
        {
            m_WindowManager->NewFrame();
        }
    }

    void DeviceImpl::FrameUpdate()
    {
        std::array executors{
            GetExecutor(ExecutorType::Copy),
            GetExecutor(ExecutorType::Compute),
            GetExecutor(ExecutorType::Graphics)
        };

        Co::manual_executor_loop_until_empty(GetExecutor(ExecutorType::Resources));
        Co::manual_executor_loop_distribute(executors, m_ConcurrentLoopCount, m_ConcurrentLoopChunkSize);
        Co::manual_executor_loop_until_empty(GetExecutor(ExecutorType::Primary));

        auto& nri     = m_DeviceWrapper->GetNri();
        auto& nriCore = *nri.GetCoreInterface();
        for (auto queueType : { CommandQueueType::COPY, CommandQueueType::COMPUTE, CommandQueueType::GRAPHICS })
        {
            auto& commandQueue = m_DeviceWrapper->GetQueue(queueType);
            m_CommandListManager.SubmitAllCommands(nriCore, commandQueue, queueType);
        }
    }

    void DeviceImpl::FrameEnd()
    {
        auto& nri          = m_DeviceWrapper->GetNri();
        auto& nriCore      = *nri.GetCoreInterface();
        auto& commandQueue = m_DeviceWrapper->GetGraphicsQueue();

        if (m_WindowManager) [[likely]]
        {
            m_WindowManager->Present();
        }

        m_FrameManager.AdvanceFrame(nriCore, commandQueue);
    }

    //

    void DeviceImpl::AssertInRenderingThread(
        bool InThread) const
    {
#ifndef AME_DIST
        bool isRenderingThread = s_RenderingThreadId == std::this_thread::get_id();
        if (isRenderingThread != InThread) [[unlikely]]
        {
            Log::Rhi().Fatal("The current thread {} be the rendering thread", InThread ? "must" : "must not");
        }
#endif
    }
} // namespace Ame::Rhi