#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Rhi/Device/Swapchain/WindowManager.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    Device::Device() = default;

    Device::Device(
        Co::runtime&            runtime,
        const DeviceCreateDesc& desc) :
        m_Impl(std::make_unique<DeviceImpl>(runtime, desc))
    {
    }

    Device::~Device() = default;

    //

    bool Device::IsHeadless() const noexcept
    {
        return m_Impl ? m_Impl->GetWindowManager() != nullptr : true;
    }

    Ptr<Co::executor> Device::GetExecutor(
        ExecutorType type) const
    {
        return m_Impl->GetExecutor(type);
    }

    //

    GraphicsAPI Device::GetGraphicsAPI() const
    {
        return m_Impl ? m_Impl->GetGraphicsAPI() : GraphicsAPI::Null;
    }

    const char* Device::GetGraphicsAPIName() const
    {
        switch (GetGraphicsAPI())
        {
        case GraphicsAPI::Null:
            return "null";
        case GraphicsAPI::DirectX12:
            return "d3d12";
        case GraphicsAPI::Vulkan:
            return "vk";
        default:
            std::unreachable();
        }
    }

    const DeviceDesc& Device::GetDesc() const
    {
        return m_Impl->GetDesc();
    }

    //

    uint64_t Device::GetFrameCount() const
    {
        auto& frameManager = m_Impl->GetFrameManager();
        return frameManager.GetFrameCount();
    }

    uint8_t Device::GetFrameIndex() const
    {
        auto& frameManager = m_Impl->GetFrameManager();
        return frameManager.GetFrameIndex();
    }

    uint8_t Device::GetFrameCountInFlight() const
    {
        auto& frameManager = m_Impl->GetFrameManager();
        return frameManager.GetFrameCountInFlight();
    }

    uint32_t Device::GetDrawIndexedCommandSize() const
    {
        return m_Impl->GetDrawIndexedCommandSize();
    }

    //

    bool Device::BeginFrame()
    {
        return m_Impl->BeginFrame();
    }

    void Device::ProcessTasks()
    {
        m_Impl->ProcessTasks();
    }

    void Device::EndFrame()
    {
        m_Impl->EndFrame();
    }

    Co::result<void> Device::WaitIdle()
    {
        return m_Impl->WaitIdle();
    }

    //

    DeviceResourceAllocator& Device::GetResourceAllocator()
    {
        return m_Impl->GetResourceAllocator();
    }

    DeviceCommandSubmitter& Device::GetCommandSubmitter()
    {
        return m_Impl->GetCommandSubmitter();
    }

    DeviceWindowManager& Device::GetWindowManager()
    {
        return m_Impl->GetDeviceWindowManager();
    }
} // namespace Ame::Rhi