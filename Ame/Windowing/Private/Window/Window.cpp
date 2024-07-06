#include <Window/Window.hpp>
#include <Window/GlfwContext.hpp>

#include <boost/predef.h>

#ifdef BOOST_OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(BOOST_OS_MACOS)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(BOOST_OS_LINUX)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>

#include <Log/Wrapper.hpp>

namespace Ame::Windowing
{
    Window::Window(
        const WindowDesc& windowDesc) :
        m_Title(windowDesc.Title)
    {
        GlfwContext::Initialize(windowDesc.PreviousContext);
        GlfwContext::Get()
            .PushTask([this, windowDesc]
                      { CreateGlfwWindow(windowDesc); })
            .wait();
    }

    Window::~Window()
    {
        ReleaseGlfwWindow();
    }

    GLFWwindow* Window::GetHandle() const
    {
        return m_Handle;
    }

    void* Window::GetNativeHandle() const
    {
#ifdef BOOST_OS_WINDOWS
        return glfwGetWin32Window(m_Handle);
#elif defined(BOOST_OS_MACOS)
        return std::bit_cast<void*>(glfwGetCocoaWindow(m_Handle));
#elif defined(BOOST_OS_LINUX)
        return std::bit_cast<void*>(glfwGetX11Window(m_Handle));
#endif
    }

    void Window::Close()
    {
        GlfwContext::Get()
            .PushTask([this]
                      { glfwSetWindowShouldClose(m_Handle, GLFW_TRUE); })
            .wait();
    }

    bool Window::IsRunning() const
    {
        return GlfwContext::Get()
                   .PushTask([this]
                             { return glfwWindowShouldClose(m_Handle); })
                   .get() == GLFW_FALSE;
    }

    const String& Window::GetTitle() const
    {
        return m_Title;
    }

    void Window::SetTitle(
        String title)
    {
        m_Title = title;
        GlfwContext::Get()
            .PushTask([this]
                      { glfwSetWindowTitle(m_Handle, m_Title.c_str()); })
            .wait();
    }

    Math::Vector2I Window::GetPosition() const
    {
        int x, y;
        GlfwContext::Get()
            .PushTask([&]
                      { glfwGetWindowPos(m_Handle, &x, &y); })
            .wait();
        return { x, y };
    }

    void Window::SetPosition(
        const Math::Vector2I& position)
    {
        GlfwContext::Get()
            .PushTask([&]
                      { glfwSetWindowPos(m_Handle, position.x(), position.y()); })
            .wait();
    }

    void Window::SetFullscreen(
        bool state)
    {
        GlfwContext::Get()
            .PushTask(
                [this, state]
                {
                    if (state)
                    {
                        GLFWmonitor*       monitor = glfwGetPrimaryMonitor();
                        const GLFWvidmode* mode    = glfwGetVideoMode(monitor);

                        m_WindowSize = { mode->width, mode->height };
                        glfwSetWindowMonitor(m_Handle, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                        glfwSetWindowAttrib(m_Handle, GLFW_DECORATED, GLFW_FALSE);
                    }
                    else
                    {
                        glfwSetWindowMonitor(m_Handle, nullptr, 0, 0, m_WindowSize.Width(), m_WindowSize.Height(), 0);
                        glfwSetWindowAttrib(m_Handle, GLFW_DECORATED, GLFW_TRUE);
                    }
                })
            .wait();
    }

    Math::Size2I Window::GetSize() const
    {
        return m_WindowSize;
    }

    bool Window::IsMinimized() const
    {
        return GlfwContext::Get()
                   .PushTask([this]
                             { return glfwGetWindowAttrib(m_Handle, GLFW_ICONIFIED); })
                   .get() == GLFW_TRUE;
    }

    void Window::Minimize()
    {
        glfwIconifyWindow(m_Handle);
    }

    void Window::Restore()
    {
        GlfwContext::Get()
            .PushTask([this]
                      { glfwRestoreWindow(m_Handle); })
            .wait();
    }

    bool Window::IsMaximized() const
    {
        return GlfwContext::Get()
                   .PushTask([this]
                             { return glfwGetWindowAttrib(m_Handle, GLFW_MAXIMIZED); })
                   .get() == GLFW_TRUE;
    }

    void Window::Maximize()
    {
        GlfwContext::Get()
            .PushTask([this]
                      { glfwMaximizeWindow(m_Handle); })
            .wait();
    }

    bool Window::IsFullScreen() const
    {
        return GlfwContext::Get()
            .PushTask([this]
                      { return glfwGetWindowMonitor(m_Handle) != nullptr; })
            .get();
    }

    bool Window::IsVisible() const
    {
        return GlfwContext::Get()
            .PushTask([this]
                      { return glfwGetWindowAttrib(m_Handle, GLFW_VISIBLE) == GLFW_TRUE &&
                               glfwGetWindowAttrib(m_Handle, GLFW_ICONIFIED) == GLFW_FALSE &&
                               glfwWindowShouldClose(m_Handle) == GLFW_FALSE; })
            .get();
    }

    void Window::SetSize(
        const Math::Size2I& size)
    {
        m_WindowSize = size;
        GlfwContext::Get()
            .PushTask([this]
                      { glfwSetWindowSize(m_Handle, m_WindowSize.Width(), m_WindowSize.Height()); })
            .wait();
    }

    void Window::SetIcon(
        const std::byte*      iconData,
        const Math::Vector2I& size)
    {
        // Set icon for window
        GLFWimage glfwImage{
            .width  = size.x(),
            .height = size.y(),
            .pixels = const_cast<uint8_t*>(std::bit_cast<const uint8_t*>(iconData))
        };
        GlfwContext::Get()
            .PushTask([this, &glfwImage]
                      { glfwSetWindowIcon(m_Handle, 1, &glfwImage); })
            .wait();
    }

    void Window::SetVisible(
        bool show)
    {
        GlfwContext::Get()
            .PushTask(
                [this, show]
                {
                    if (show)
                    {
                        glfwShowWindow(m_Handle);
                    }
                    else
                    {
                        glfwHideWindow(m_Handle);
                    }
                })
            .wait();
    }

    void Window::RequestFocus()
    {
        GlfwContext::Get()
            .PushTask([this]
                      { glfwRequestWindowAttention(m_Handle); })
            .wait();
    }

    bool Window::HasFocus() const
    {
        return GlfwContext::Get()
            .PushTask([this]
                      { return glfwGetWindowAttrib(m_Handle, GLFW_FOCUSED) == GLFW_TRUE; })
            .get();
    }

    void Window::ProcessEvents()
    {
        GlfwContext::Get()
            .PushTask([this]
                      { glfwPollEvents(); })
            .wait();
    }

    //

    void Window::CreateGlfwWindow(
        const WindowDesc& windowDesc)
    {
        GLFWmonitor*       monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode    = glfwGetVideoMode(monitor);

        {
            glfwDefaultWindowHints();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#ifdef AME_PLATFORM_WINDOWS
            glfwWindowHint(GLFW_TITLEBAR, !windowDesc.CustomTitleBar);
#else
            glfwWindowHint(GLFW_DECORATED, !windowDesc.CustomTitleBar);
#endif

            if (windowDesc.FullScreen)
            {
                m_Handle = glfwCreateWindow(mode->width, mode->height, windowDesc.Title, monitor, nullptr);
            }
            else
            {
                m_Handle = glfwCreateWindow(windowDesc.Size.Width(), windowDesc.Size.Height(), windowDesc.Title, nullptr, nullptr);
            }
        }

        if (windowDesc.FullScreen)
        {
            if (windowDesc.StartInMiddle)
            {
                int x = (mode->width - windowDesc.Size.Width()) / 2;
                int y = (mode->height - windowDesc.Size.Height()) / 2;

                glfwSetWindowPos(m_Handle, x, y);
            }

            if (windowDesc.Maximized)
            {
                glfwMaximizeWindow(m_Handle);
            }
        }

        if (windowDesc.NoResize)
        {
            glfwSetWindowAttrib(m_Handle, GLFW_RESIZABLE, GLFW_FALSE);
        }

        glfwSetWindowUserPointer(m_Handle, this);
        glfwGetWindowSize(m_Handle, &m_WindowSize.x(), &m_WindowSize.y());

        //

        glfwSetWindowSizeCallback(
            m_Handle,
            [](GLFWwindow* glfwWindow, int width, int height)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                if (width && height)
                {
                    window->m_OnWindowSizeChanged(window->m_WindowSize);
                }
                window->m_WindowSize = Math::Size2I{ width, height };
            });

        glfwSetWindowCloseCallback(
            m_Handle,
            [](GLFWwindow* glfwWindow)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                window->m_OnWindowClosed();
            });

        if (windowDesc.CustomTitleBar)
        {
            glfwSetTitlebarHitTestCallback(
                m_Handle,
                [](GLFWwindow* glfwWindow, int x, int y, int* hit)
                {
                    auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                    *hit        = window->m_OnWindowTitleHitTest({ x, y }).value_or(false);
                });
        }

        glfwSetWindowIconifyCallback(
            m_Handle,
            [](GLFWwindow* glfwWindow, int iconified)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                bool wasHit = false;
                window->m_OnWindowMinized(iconified);
            });
    }

    void Window::ReleaseGlfwWindow()
    {
        if (m_Handle)
        {
            GlfwContext::Get()
                .PushTask([this]
                          { glfwDestroyWindow(m_Handle); })
                .wait();
        }
    }
} // namespace Ame::Windowing