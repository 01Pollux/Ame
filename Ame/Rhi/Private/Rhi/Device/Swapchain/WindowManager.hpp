#pragma once

#include <Core/Ame.hpp>

#include <Window/Window.hpp>
#include <Rhi/Device/Device.hpp>
#include <Rhi/Resource/Backbuffer.hpp>

#include <Rhi/Nri/Bridge.hpp>

namespace Ame::Rhi
{
    class IDeviceWrapper;

    class WindowManager
    {
    public:
        WindowManager(
            DeviceResourceAllocator& resourceAllocator,
            const DeviceCreateDesc&  deviceCreateDesc);

        WindowManager(const WindowManager&) = delete;
        WindowManager(WindowManager&&)      = delete;

        WindowManager& operator=(const WindowManager&) = delete;
        WindowManager& operator=(WindowManager&&)      = delete;

        ~WindowManager();

    public:
        /// <summary>
        /// Mark the start of frame.
        /// </summary>
        void NewFrame();

        /// <summary>
        /// Present the current frame.
        /// </summary>
        void Present();

    public:
        /// <summary>
        /// The window of the device
        /// </summary>
        [[nodiscard]] Windowing::Window* GetWindow();

        /// <summary>
        /// Get the nri device
        /// </summary>
        [[nodiscard]] nri::SwapChain& GetSwapChain();

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
        /// Get the number of backbuffers.
        /// This is the number of backbuffers that are used for rendering.
        /// </summary>
        [[nodiscard]] uint8_t GetBackbufferCount() const;

        /// <summary>
        /// Get the index of the current backbuffer.
        /// This is the index of the backbuffer that is currently being rendered to.
        /// </summary>
        [[nodiscard]] uint8_t GetBackbufferIndex() const;

        /// <summary>
        /// Get the backbuffer at the specified index.
        /// </summary>
        [[nodiscard]] const Backbuffer& GetBackbuffer(
            uint8_t index) const;

    public:
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
        /// Check if the swapchain needs to be recreated
        /// </summary>
        [[nodiscard]] bool IsDirty() const noexcept;

        /// <summary>
        /// Create the swapchain for the device
        /// </summary>
        void RecreateSwapchain();

    private:
        /// <summary>
        /// Release and recreate the swapchain views
        /// </summary>
        void CreateSwapChainViews();

        /// <summary>
        /// Release the swapchain views
        /// </summary>
        void ReleaseBackBuffers();

    private:
        Windowing::Window m_Window;

        Ref<DeviceResourceAllocator> m_ResourceAllocator;
        nri::SwapChain*              m_SwapChain = nullptr;

        Signals::ScopedConnection m_OnWindowSizeChanged;

        std::vector<Backbuffer> m_BackBuffers;
        uint32_t                m_BackBufferIndex = 0;

        nri::SwapChainFormat m_SwapChainFormat  = nri::SwapChainFormat::MAX_NUM;
        nri::Format          m_BackBufferFormat = nri::Format::UNKNOWN;

        bool m_DirtySwapChain : 1 = false;
        bool m_VSyncEnabled   : 1 = false;
    };
} // namespace Ame::Rhi