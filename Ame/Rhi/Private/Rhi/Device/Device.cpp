#include <Rhi/Device/Device.hpp>

#include <DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    RhiDevice::RhiDevice() = default;

    RhiDevice::RhiDevice(
        Co::runtime&            runtime,
        const DeviceCreateDesc& createDesc) :
        m_ConcurrentLoopCount(createDesc.ConcurrentLoopCount),
        m_ConcurrentLoopChunkSize(createDesc.ConcurrentLoopChunkSize)
    {
        auto deviceWrapper = DeviceWrapper::Create(createDesc);
        if (!deviceWrapper)
        {
            Log::Rhi().Fatal("Failed to create the device wrapper.");
        }
        else
        {
            m_Wrapper = std::move(*deviceWrapper);
        }

        for (auto& executor : m_Executors)
        {
            executor = runtime.make_manual_executor();
        }
    }

    RhiDevice::~RhiDevice() = default;

    //

    bool RhiDevice::IsNull() const noexcept
    {
        return !m_Wrapper;
    }

    bool RhiDevice::IsHeadless() const noexcept
    {
        return m_Wrapper && !m_Wrapper.GetWindowWrapper();
    }

    bool RhiDevice::HasSurface() const noexcept
    {
        return m_Wrapper && m_Wrapper.GetWindowWrapper();
    }

    const Ptr<Co::manual_executor>& RhiDevice::GetExecutor(
        ExecutorType type) const
    {
        return m_Executors[static_cast<size_t>(type)];
    }

    //

    Dg::RENDER_DEVICE_TYPE RhiDevice::GetGraphicsAPI() const
    {
        Dg::RENDER_DEVICE_TYPE deviceType = Dg::RENDER_DEVICE_TYPE_UNDEFINED;
        if (m_Wrapper)
        {
            deviceType = GetDevice()->GetDeviceInfo().Type;
        }
        return deviceType;
    }

    const char* RhiDevice::GetGraphicsAPIName() const
    {
        return Dg::GetRenderDeviceTypeShortString(GetGraphicsAPI());
    }

    //

    Dg::IEngineFactory* RhiDevice::GetFactory() const
    {
        return m_Wrapper.GetFactory();
    }

    Dg::IRenderDevice* RhiDevice::GetDevice() const
    {
        return m_Wrapper.GetDevice();
    }

    Dg::IDeviceContext* RhiDevice::GetImmediateContext() const
    {
        return m_Wrapper.GetImmediateContext();
    }

    Dg::ISwapChain* RhiDevice::GetSwapchain() const
    {
        return m_Wrapper.GetWindowWrapper().GetSwapchain();
    }

    Ptr<Windowing::Window> RhiDevice::GetWindow() const
    {
        return m_Wrapper.GetWindowWrapper().GetWindow();
    }

    //

    bool RhiDevice::ProcessTasks()
    {
        bool isRunning = true;
        if (!IsNull()) [[likely]]
        {
            auto& windowWrapper = m_Wrapper.GetWindowWrapper();
            isRunning           = !windowWrapper || windowWrapper.PollEvents();

            std::array executors{
                GetExecutor(ExecutorType::Copy),
                GetExecutor(ExecutorType::Compute),
                GetExecutor(ExecutorType::Graphics)
            };

            Co::manual_executor_loop_until_empty(GetExecutor(ExecutorType::Resources));
            Co::manual_executor_loop_distribute(executors, m_ConcurrentLoopCount, m_ConcurrentLoopChunkSize);
            Co::manual_executor_loop_until_empty(GetExecutor(ExecutorType::Primary));
        }
        return isRunning;
    }

    void RhiDevice::AdvanceFrame(
        uint32_t syncInterval)
    {
        if (!IsNull()) [[likely]]
        {
            auto& windowWrapper = m_Wrapper.GetWindowWrapper();
            if (windowWrapper)
            {
                windowWrapper.Present(syncInterval);
            }
            else
            {
                GetImmediateContext()->FinishFrame();
            }
        }
    }
} // namespace Ame::Rhi