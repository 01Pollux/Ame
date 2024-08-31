#include <Window/Window.hpp>
#include <Window/Glfw/WindowImplGlfw.hpp>
#include <Window/Glfw/GlfwContext.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Window
{
    WindowImplGlfw::WindowImplGlfw(IReferenceCounters* referenceCounters, const WindowCreateDesc& windowDesc) :
        Base(referenceCounters), m_Title(windowDesc.Title)
    {
        GlfwContext::Initialize();
        GlfwContext::Get().PushTask([this, windowDesc] { CreateGlfwWindow(windowDesc); }).wait();

        m_ImGuiWindow = ObjectAllocator<ImGuiWindowImplGlfw>()(m_Handle);
    }

    WindowImplGlfw::~WindowImplGlfw()
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

    void WindowImplGlfw::ProcessEvents()
    {
        GlfwContext::Get().PushTask([this] { glfwPollEvents(); }).wait();
    }

    bool WindowImplGlfw::IsRunning() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwWindowShouldClose(m_Handle); }).get() == GLFW_FALSE;
    }

    void WindowImplGlfw::Close()
    {
        GlfwContext::Get().PushTask([this] { glfwSetWindowShouldClose(m_Handle, GLFW_TRUE); }).wait();
    }

    //

    void WindowImplGlfw::SetTitle(const String& title)
    {
        m_Title = title;
        GlfwContext::Get().PushTask([this] { glfwSetWindowTitle(m_Handle, m_Title.c_str()); }).wait();
    }

    void WindowImplGlfw::SetSize(const Math::Size2I& size)
    {
        m_WindowSize = size;
        GlfwContext::Get()
            .PushTask([this] { glfwSetWindowSize(m_Handle, m_WindowSize.Width(), m_WindowSize.Height()); })
            .wait();
    }

    void WindowImplGlfw::SetPosition(const Math::Vector2I& position)
    {
        GlfwContext::Get().PushTask([&] { glfwSetWindowPos(m_Handle, position.x(), position.y()); }).wait();
    }

    //

    const String& WindowImplGlfw::GetTitle() const
    {
        return m_Title;
    }

    const Math::Size2I& WindowImplGlfw::GetSize() const
    {
        return m_WindowSize;
    }

    Math::Vector2I WindowImplGlfw::GetPosition() const
    {
        int x, y;
        GlfwContext::Get().PushTask([&] { glfwGetWindowPos(m_Handle, &x, &y); }).wait();
        return { x, y };
    }

    //

    GLFWwindow* WindowImplGlfw::GetGlfwHandle() const
    {
        return m_Handle;
    }

    void WindowImplGlfw::SetFullscreen(bool state)
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

    bool WindowImplGlfw::IsMinimized() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwGetWindowAttrib(m_Handle, GLFW_ICONIFIED); }).get() ==
               GLFW_TRUE;
    }

    bool WindowImplGlfw::IsMaximized() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwGetWindowAttrib(m_Handle, GLFW_MAXIMIZED); }).get() ==
               GLFW_TRUE;
    }

    void WindowImplGlfw::Minimize()
    {
        glfwIconifyWindow(m_Handle);
    }

    void WindowImplGlfw::Maximize()
    {
        GlfwContext::Get().PushTask([this] { glfwMaximizeWindow(m_Handle); }).wait();
    }

    bool WindowImplGlfw::IsFullScreen() const
    {
        return GlfwContext::Get().PushTask([this] { return glfwGetWindowMonitor(m_Handle) != nullptr; }).get();
    }

    bool WindowImplGlfw::IsVisible() const
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

    bool WindowImplGlfw::HasFocus() const
    {
        return GlfwContext::Get()
            .PushTask([this] { return glfwGetWindowAttrib(m_Handle, GLFW_FOCUSED) == GLFW_TRUE; })
            .get();
    }

    void WindowImplGlfw::RequestFocus()
    {
        GlfwContext::Get().PushTask([this] { glfwRequestWindowAttention(m_Handle); }).wait();
    }

    void WindowImplGlfw::SetVisible(bool show)
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

    void WindowImplGlfw::Restore()
    {
        GlfwContext::Get().PushTask([this] { glfwRestoreWindow(m_Handle); }).wait();
    }

    WindowEventListener& WindowImplGlfw::GetEventListener()
    {
        return m_EventListener;
    }

    //

    void WindowImplGlfw::CreateGlfwWindow(const WindowCreateDesc& windowDesc)
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
                auto         window = static_cast<WindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                Math::Size2I newSize{ width, height };
                if (width && height)
                {
                    window->GetEventListener().Invoke_OnWindowSizeChanged(newSize);
                }
                window->m_WindowSize = Math::Size2I{ width, height };
                return true;
            });

        m_WindowPosCallbackId = GlfwContext::GetHooks().Install_WindowClose(
            m_Handle,
            [](GLFWwindow* glfwWindow)
            {
                auto window = static_cast<WindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                window->GetEventListener().Invoke_OnWindowClosed();
                return true;
            });

        if (windowDesc.CustomTitleBar)
        {
            m_WindowTitlebarCallbackId = GlfwContext::GetHooks().Install_TitlebarHitTest(
                m_Handle,
                [](GLFWwindow* glfwWindow, int x, int y, int* hit)
                {
                    auto window = static_cast<WindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                    *hit =
                        window->GetEventListener().Invoke_OnWindowTitleHitTest(Math::Vector2I{ x, y }).value_or(false);
                    return true;
                });
        }

        m_WindowIconifyCallbackId = GlfwContext::GetHooks().Install_WindowIconified(
            m_Handle,
            [](GLFWwindow* glfwWindow, int iconified)
            {
                auto window = static_cast<WindowImplGlfw*>(glfwGetWindowUserPointer(glfwWindow));
                window->GetEventListener().Invoke_OnWindowMinized(iconified);
                return true;
            });
    }
} // namespace Ame::Window