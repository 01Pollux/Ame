#include <Rhi/Wrapper/WindowWrapper.hpp>
#include <Rhi/Device/RhiDeviceCreateDesc.hpp>

#include <Window/Window.hpp>

namespace Ame::Rhi
{
    WindowWrapper::WindowWrapper(
        Ptr<Window::IWindow> window,
        Ptr<Dg::ISwapChain>  swapchain) :
        m_Window(std::move(window)),
        m_Swapchain(std::move(swapchain)),

        m_WindowSizeChangedConnection(
            m_Window->GetEventListener().OnWindowSizeChanged(
                [this](const Math::Size2I&)
                { m_DirtySwapChain = true; }))
    {
    }

    //

    bool WindowWrapper::PollEvents()
    {
        m_Window->ProcessEvents();
        return m_Window->IsRunning();
    }

    void WindowWrapper::Present(
        uint32_t syncInterval)
    {
        m_Swapchain->Present(syncInterval);

        if (m_DirtySwapChain)
        {
            auto size = m_Window->GetSize();
            m_Swapchain->Resize(size.x(), size.y());
        }
    }

    //

    Window::IWindow* WindowWrapper::GetWindow() const
    {
        return m_Window;
    }

    Dg::ISwapChain* WindowWrapper::GetSwapchain() const
    {
        return m_Swapchain;
    }
} // namespace Ame::Rhi