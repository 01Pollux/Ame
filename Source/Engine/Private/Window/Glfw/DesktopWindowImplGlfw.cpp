#include <Window/Window.hpp>
#include <Window/Glfw/DesktopWindowImplGlfw.hpp>
#include <Window/Glfw/GlfwDriverImpl.hpp>

namespace Ame::Window
{
    DesktopWindowImplGlfw::DesktopWindowImplGlfw(IReferenceCounters* counters, const WindowCreateDesc& windowDesc) :
        Base(counters), m_Title(windowDesc.Title)
    {
        GlfwDriverImpl::Initialize();
        GetGlfwDriver()->submit([this, windowDesc] { CreateGlfwWindow(windowDesc); }).wait();
    }

    DesktopWindowImplGlfw::~DesktopWindowImplGlfw()
    {
        GetGlfwDriver()
            ->submit(
                [this]
                {
                    GlfwHooks::Get().Uninstall_WindowSize(m_Handle, m_WindowSizeCallbackId);
                    GlfwHooks::Get().Uninstall_WindowPos(m_Handle, m_WindowPosCallbackId);
                    GlfwHooks::Get().Uninstall_TitlebarHitTest(m_Handle, m_WindowTitlebarCallbackId);
                    GlfwHooks::Get().Uninstall_WindowIconified(m_Handle, m_WindowIconifyCallbackId);
                    glfwDestroyWindow(m_Handle);
                })
            .wait();
    }

    //

    bool DesktopWindowImplGlfw::IsRunning() const
    {
        return GetGlfwDriver()->submit([this] { return glfwWindowShouldClose(m_Handle); }).get() == GLFW_FALSE;
    }

    void DesktopWindowImplGlfw::Close()
    {
        GetGlfwDriver()->submit([this] { glfwSetWindowShouldClose(m_Handle, GLFW_TRUE); }).wait();
    }

    //

    void DesktopWindowImplGlfw::SetTitle(const String& title)
    {
        m_Title = title;
        GetGlfwDriver()->submit([this] { glfwSetWindowTitle(m_Handle, m_Title.c_str()); }).wait();
    }

    void DesktopWindowImplGlfw::SetSize(const Math::Size2I& size)
    {
        m_WindowSize = size;
        GetGlfwDriver()
            ->submit([this] { glfwSetWindowSize(m_Handle, m_WindowSize.Width(), m_WindowSize.Height()); })
            .wait();
    }

    void DesktopWindowImplGlfw::SetPosition(const Math::Vector2I& position)
    {
        GetGlfwDriver()->submit([&] { glfwSetWindowPos(m_Handle, position.x(), position.y()); }).wait();
    }

    //

    const String& DesktopWindowImplGlfw::GetTitle() const
    {
        return m_Title;
    }

    const Math::Size2I& DesktopWindowImplGlfw::GetSize() const
    {
        return m_WindowSize;
    }

    Math::Vector2I DesktopWindowImplGlfw::GetPosition() const
    {
        int x, y;
        GetGlfwDriver()->submit([&] { glfwGetWindowPos(m_Handle, &x, &y); }).wait();
        return { x, y };
    }

    //

    GLFWwindow* DesktopWindowImplGlfw::GetGlfwHandle() const
    {
        return m_Handle;
    }

    void DesktopWindowImplGlfw::SetFullscreen(bool state)
    {
        GetGlfwDriver()
            ->submit(
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

    bool DesktopWindowImplGlfw::IsMinimized() const
    {
        return GetGlfwDriver()->submit([this] { return glfwGetWindowAttrib(m_Handle, GLFW_ICONIFIED); }).get() ==
               GLFW_TRUE;
    }

    bool DesktopWindowImplGlfw::IsMaximized() const
    {
        return GetGlfwDriver()->submit([this] { return glfwGetWindowAttrib(m_Handle, GLFW_MAXIMIZED); }).get() ==
               GLFW_TRUE;
    }

    void DesktopWindowImplGlfw::Minimize()
    {
        GetGlfwDriver()->submit([this] { glfwIconifyWindow(m_Handle); }).wait();
    }

    void DesktopWindowImplGlfw::Maximize()
    {
        GetGlfwDriver()->submit([this] { glfwMaximizeWindow(m_Handle); }).wait();
    }

    bool DesktopWindowImplGlfw::IsFullScreen() const
    {
        return GetGlfwDriver()->submit([this] { return glfwGetWindowMonitor(m_Handle) != nullptr; }).get();
    }

    bool DesktopWindowImplGlfw::IsVisible() const
    {
        return GetGlfwDriver()
            ->submit(
                [this]
                {
                    return glfwGetWindowAttrib(m_Handle, GLFW_VISIBLE) == GLFW_TRUE &&
                           glfwGetWindowAttrib(m_Handle, GLFW_ICONIFIED) == GLFW_FALSE &&
                           glfwWindowShouldClose(m_Handle) == GLFW_FALSE;
                })
            .get();
    }

    bool DesktopWindowImplGlfw::HasFocus() const
    {
        return GetGlfwDriver()
            ->submit([this] { return glfwGetWindowAttrib(m_Handle, GLFW_FOCUSED) == GLFW_TRUE; })
            .get();
    }

    void DesktopWindowImplGlfw::RequestFocus()
    {
        GetGlfwDriver()->submit([this] { glfwRequestWindowAttention(m_Handle); }).wait();
    }

    void DesktopWindowImplGlfw::SetVisible(bool show)
    {
        GetGlfwDriver()
            ->submit(
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

    //

    void DesktopWindowImplGlfw::Restore()
    {
        GetGlfwDriver()->submit([this] { glfwRestoreWindow(m_Handle); }).wait();
    }

    WindowEventListener& DesktopWindowImplGlfw::GetEventListener()
    {
        return m_EventListener;
    }

    SharedPtr<IGlfwDriver> DesktopWindowImplGlfw::GetGlfwDriver() const
    {
        return GlfwDriverImpl::Initialize();
    }

    //

    void DesktopWindowImplGlfw::CreateGlfwWindow(const WindowCreateDesc& windowDesc)
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
                m_Handle = glfwCreateWindow(
                    windowDesc.Size.Width(), windowDesc.Size.Height(), windowDesc.Title, nullptr, nullptr);
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

        m_WindowSizeCallbackId = GlfwHooks::Get().Install_WindowSize(
            m_Handle,
            [](GLFWwindow* glfwWindow, int width, int height)
            {
                auto         window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                Math::Size2I newSize{ width, height };
                window->m_WindowSize = Math::Size2I{ width, height };

                if (glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED) == GLFW_FALSE)
                {
                    if (newSize.Width() && newSize.Height())
                    {
                        window->GetEventListener().OnWindowSizeChanged.Invoke(newSize);
                    }
                }

                return true;
            });

        m_WindowPosCallbackId = GlfwHooks::Get().Install_WindowClose(
            m_Handle,
            [](GLFWwindow* glfwWindow)
            {
                auto window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                window->GetEventListener().OnWindowClosed.Invoke();
                return true;
            });

        if (windowDesc.CustomTitleBar)
        {
            m_WindowTitlebarCallbackId = GlfwHooks::Get().Install_TitlebarHitTest(
                m_Handle,
                [](GLFWwindow* glfwWindow, int x, int y, int* hit)
                {
                    auto window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));

                    *hit =
                        window->GetEventListener().OnWindowTitleHitTest.Invoke(Math::Vector2I{ x, y }).value_or(false);
                    return true;
                });
        }

        m_WindowIconifyCallbackId = GlfwHooks::Get().Install_WindowIconified(
            m_Handle,
            [](GLFWwindow* glfwWindow, int iconified)
            {
                auto window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                window->GetEventListener().OnWindowMinized.Invoke(iconified);
                return true;
            });
    }
} // namespace Ame::Window