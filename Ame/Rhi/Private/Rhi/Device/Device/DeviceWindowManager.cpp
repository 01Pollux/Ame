#include <Rhi/Device/Swapchain/WindowManager.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>
#include <Rhi/Device/WindowManager.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    DeviceWindowManager::DeviceWindowManager(
        DeviceImpl& rhiDeviceImpl) :
        m_RhiDeviceImpl(rhiDeviceImpl)
    {
    }

    //

    bool DeviceWindowManager::IsHeadless() const
    {
        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return windowManager != nullptr;
    }

    bool DeviceWindowManager::IsVSyncEnabled() const noexcept
    {
        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return IsHeadless() ? false : windowManager->IsVSyncEnabled();
    }

    void DeviceWindowManager::SetVSyncEnabled(
        bool state)
    {
        if (!IsHeadless()) [[likely]]
        {
            auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
            windowManager->SetVSyncEnabled(state);
        }
    }

    //

    Windowing::Window& DeviceWindowManager::GetWindow() const
    {
        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return *windowManager->GetWindow();
    }

    SwapChainFormat DeviceWindowManager::GetSwapchainFormat() const
    {
        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return windowManager->GetSwapchainFormat();
    }

    ResourceFormat DeviceWindowManager::GetBackbufferFormat() const
    {
        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return windowManager->GetBackbufferFormat();
    }

    //

    uint8_t DeviceWindowManager::GetBackbufferCount() const
    {
        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return windowManager->GetBackbufferCount();
    }

    uint8_t DeviceWindowManager::GetBackbufferIndex() const
    {
        m_RhiDeviceImpl.get().AssertRenderingThread();

        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return windowManager->GetBackbufferIndex();
    }

    CRef<Backbuffer> DeviceWindowManager::GetBackbuffer(
        uint8_t index) const
    {
        m_RhiDeviceImpl.get().AssertRenderingThread();

        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return windowManager->GetBackbuffer(index);
    }

    CRef<Backbuffer> DeviceWindowManager::GetBackbuffer() const
    {
        m_RhiDeviceImpl.get().AssertRenderingThread();

        auto windowManager = m_RhiDeviceImpl.get().GetWindowManager();
        return windowManager->GetBackbuffer(windowManager->GetBackbufferIndex());
    }
} // namespace Ame::Rhi