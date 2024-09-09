#include <Window/Window.hpp>
#include <Window/Glfw/DesktopWindowImplGlfw.hpp>
#include <Window/Glfw/GlfwContext.hpp>

namespace Ame::Window
{
    DesktopWindowImplGlfw::DesktopWindowImplGlfw(IReferenceCounters* counters, const WindowCreateDesc& windowDesc) :
        Base(counters), m_Title(windowDesc.Title)
    {
        GlfwContext::Initialize();
        GlfwContext::Get().PushTask([this, windowDesc] { CreateGlfwWindow(windowDesc); }).wait();

        m_ImGuiWindow = AmeCreate(ImGuiWindowImplGlfw, m_Handle);
    }

    DesktopWindowImplGlfw::~DesktopWindowImplGlfw()
    {
        GlfwContext::Get()
            .PushTask(
                [this]
                {
                    GlfwContext::GetHooks().Uninstall_WindowSize(m_Handle, m_WindowSizeCallbackId);
                    GlfwContext::GetHooks().Uninstall_WindowPos(m_Handle, m_WindowPosCallbackId);
                    GlfwContext::GetHooks().Uninstall_TitlebarHitTest(m_Handle, m_WindowTitlebarCallbackId);
                    GlfwContext::GetHooks().Uninstall_WindowIconified(m_Handle, m_WindowIconifyCallbackId);
                    glfwDestroyWindow(m_Handle);
                })
            .wait();
    }

    //

    void DesktopWindowImplGlfw::ProcessEvents()
    {
        GlfwContext::Get().PushTask([this] { glfwPollEvents(); }).wait();
    }

    bool DesktopWindowImplGlfw::IsRunning() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwWindowShouldClose(m_Handle); }).get() == GLFW_FALSE;
    }

    void DesktopWindowImplGlfw::Close()
    {
        GlfwContext::Get().PushTask([this] { glfwSetWindowShouldClose(m_Handle, GLFW_TRUE); }).wait();
    }

    //

    void DesktopWindowImplGlfw::SetTitle(const String& title)
    {
        m_Title = title;
        GlfwContext::Get().PushTask([this] { glfwSetWindowTitle(m_Handle, m_Title.c_str()); }).wait();
    }

    void DesktopWindowImplGlfw::SetSize(const Math::Size2I& size)
    {
        m_WindowSize = size;
        GlfwContext::Get()
            .PushTask([this] { glfwSetWindowSize(m_Handle, m_WindowSize.Width(), m_WindowSize.Height()); })
            .wait();
    }

    void DesktopWindowImplGlfw::SetPosition(const Math::Vector2I& position)
    {
        GlfwContext::Get().PushTask([&] { glfwSetWindowPos(m_Handle, position.x(), position.y()); }).wait();
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
        GlfwContext::Get().PushTask([&] { glfwGetWindowPos(m_Handle, &x, &y); }).wait();
        return { x, y };
    }

    //

    GLFWwindow* DesktopWindowImplGlfw::GetGlfwHandle() const
    {
        return m_Handle;
    }

    void DesktopWindowImplGlfw::SetFullscreen(bool state)
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

    bool DesktopWindowImplGlfw::IsMinimized() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwGetWindowAttrib(m_Handle, GLFW_ICONIFIED); }).get() ==
               GLFW_TRUE;
    }

    bool DesktopWindowImplGlfw::IsMaximized() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwGetWindowAttrib(m_Handle, GLFW_MAXIMIZED); }).get() ==
               GLFW_TRUE;
    }

    void DesktopWindowImplGlfw::Minimize()
    {
        GlfwContext::Get().PushTask([this] { glfwIconifyWindow(m_Handle); }).wait();
    }

    void DesktopWindowImplGlfw::Maximize()
    {
        GlfwContext::Get().PushTask([this] { glfwMaximizeWindow(m_Handle); }).wait();
    }

    bool DesktopWindowImplGlfw::IsFullScreen() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwGetWindowMonitor(m_Handle) != nullptr; }).get();
    }

    bool DesktopWindowImplGlfw::IsVisible() const
    {
        return GlfwContext::Get()
            .PushTask(
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
        return GlfwContext::Get()
            .PushTask([this] { return glfwGetWindowAttrib(m_Handle, GLFW_FOCUSED) == GLFW_TRUE; })
            .get();
    }

    void DesktopWindowImplGlfw::RequestFocus()
    {
        GlfwContext::Get().PushTask([this] { glfwRequestWindowAttention(m_Handle); }).wait();
    }

    void DesktopWindowImplGlfw::SetVisible(bool show)
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

    //

    void DesktopWindowImplGlfw::Restore()
    {
        GlfwContext::Get().PushTask([this] { glfwRestoreWindow(m_Handle); }).wait();
    }

    WindowEventListener& DesktopWindowImplGlfw::GetEventListener()
    {
        return m_EventListener;
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

        m_WindowSizeCallbackId = GlfwContext::GetHooks().Install_WindowSize(
            m_Handle,
            [](GLFWwindow* glfwWindow, int width, int height)
            {
                auto         window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                Math::Size2I newSize{ width, height };
                if (width && height)
                {
                    window->GetEventListener().OnWindowSizeChanged.Invoke(newSize);
                }
                window->m_WindowSize = Math::Size2I{ width, height };
                return true;
            });

        m_WindowPosCallbackId = GlfwContext::GetHooks().Install_WindowClose(
            m_Handle,
            [](GLFWwindow* glfwWindow)
            {
                auto window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                window->GetEventListener().OnWindowClosed.Invoke();
                return true;
            });

        if (windowDesc.CustomTitleBar)
        {
            m_WindowTitlebarCallbackId = GlfwContext::GetHooks().Install_TitlebarHitTest(
                m_Handle,
                [](GLFWwindow* glfwWindow, int x, int y, int* hit)
                {
                    auto window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                    *hit =
                        window->GetEventListener().OnWindowTitleHitTest.Invoke(Math::Vector2I{ x, y }).value_or(false);
                    return true;
                });
        }

        m_WindowIconifyCallbackId = GlfwContext::GetHooks().Install_WindowIconified(
            m_Handle,
            [](GLFWwindow* glfwWindow, int iconified)
            {
                auto window = static_cast<DesktopWindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                window->GetEventListener().OnWindowMinized.Invoke(iconified);
                return true;
            });
    }
} // namespace Ame::Window