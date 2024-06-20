#pragma once

#include <Core/Coroutine.hpp>
#include <Rhi/Descs/Core.hpp>
#include <Rhi/Resource/Backbuffer.hpp>

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

namespace Ame::Rhi
{
    class DeviceWindowManager
    {
    public:
        DeviceWindowManager(
            class DeviceImpl& rhiDeviceImpl);

    public:
        /// <summary>
        /// Check if the device is headless.
        /// This means that the device is not rendering to a window.
        /// </summary>
        [[nodiscard]] bool IsHeadless() const;

        /// <summary>
        /// Return vsync state for our swapchain
        /// </summary>
        [[nodiscard]] bool IsVSyncEnabled() const noexcept;

        /// <summary>
        /// Return vsync state for our swapchain
        /// </summary>
        void SetVSyncEnabled(
            bool state = true);

    public:
        /// <summary>
        /// Get the window associated with the device.
        /// </summary>
        [[nodiscard]] Windowing::Window& GetWindow() const;

    public:
        /// <summary>
        /// Get the swapchain format.
        /// </summary>
        [[nodiscard]] SwapChainFormat GetSwapchainFormat() const;

        /// <summary>
        /// Get the backbuffer texture format.
        /// </summary>
        [[nodiscard]] ResourceFormat GetBackbufferFormat() const;

    public:
        /// <summary>
        /// Get the backbuffer count.
        /// </summary>
        [[nodiscard]] uint8_t GetBackbufferCount() const;

        /// <summary>
        /// Get the current backbuffer index.
        /// </summary>
        [[nodiscard]] uint8_t GetBackbufferIndex() const;

        /// <summary>
        /// Get the backbuffer at the specified index.
        /// </summary>
        [[nodiscard]] CRef<Backbuffer> GetBackbuffer(
            uint8_t index) const;

        /// <summary>
        /// Get the current backbuffer.
        /// </summary>
        [[nodiscard]] CRef<Backbuffer> GetBackbuffer() const;

    private:
        Ref<DeviceImpl> m_RhiDeviceImpl;
    };
} // namespace Ame::Rhi