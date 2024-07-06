#pragma once

#include <Rhi/Core.hpp>
#include <Signals/Window/Window.hpp>

namespace Ame::Rhi
{
    struct SwapchainDesc;
    struct FullscreenModeDesc;

    class WindowWrapper
    {
    public:
        WindowWrapper() = default;
        WindowWrapper(
            Ptr<Windowing::Window>  window,
            Dg::Ptr<Dg::ISwapChain> swapchain);

    public:
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return m_Window != nullptr;
        }

    public:
        /// <summary>
        /// Poll events.
        /// return wether the window is being closed or not.
        /// </summary>
        [[nodiscard]] bool PollEvents();

        /// <summary>
        /// Present the backbuffer.
        /// </summary>
        void Present(
            uint32_t syncInterval);

    public:
        /// <summary>
        /// Get the window associated with the swapchain.
        /// </summary>
        [[nodiscard]] Ptr<Windowing::Window> GetWindow() const;

        /// <summary>
        /// Get the swapchain.
        /// </summary>
        [[nodiscard]] Dg::ISwapChain* GetSwapchain() const;

    private:
        Ptr<Windowing::Window>  m_Window;
        Dg::Ptr<Dg::ISwapChain> m_Swapchain;

        Signals::ScopedConnection m_WindowSizeChangedConnection;

        bool m_DirtySwapChain = false;
    };
} // namespace Ame::Rhi