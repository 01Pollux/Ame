#pragma once

#include <Rhi/Core.hpp>
#include <Window/Window.hpp>
#include <Window/WindowEventListener.hpp>

namespace Ame::Rhi
{
    struct SwapchainDesc;
    struct FullscreenModeDesc;

    class WindowWrapper
    {
    public:
        WindowWrapper() = default;
        WindowWrapper(Ptr<Window::IWindow> window, Ptr<Dg::ISwapChain> swapchain);

        WindowWrapper(const WindowWrapper&)           = delete;
        WindowWrapper(WindowWrapper&& other) noexcept = default;

        WindowWrapper& operator=(const WindowWrapper&)     = delete;
        WindowWrapper& operator=(WindowWrapper&&) noexcept = default;

        ~WindowWrapper() = default;

    public:
        /// <summary>
        /// Present the backbuffer.
        /// </summary>
        void Present(uint32_t syncInterval);

    public:
        /// <summary>
        /// Get the window associated with the swapchain.
        /// </summary>
        [[nodiscard]] Window::IWindow* GetWindow() const;

        /// <summary>
        /// Get the swapchain.
        /// </summary>
        [[nodiscard]] Dg::ISwapChain* GetSwapchain() const;

    private:
        Ptr<Window::IWindow> m_Window;
        Ptr<Dg::ISwapChain>  m_Swapchain;

        Signals::ScopedConnection m_WindowSizeChangedConnection;

        bool m_DirtySwapChain = false;
    };
} // namespace Ame::Rhi