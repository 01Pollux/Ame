#include <Window/Window.hpp>
#include <GLFW/glfw3.h>
#include <mutex>

#include <Log/Wrapper.hpp>

namespace Ame::Windowing
{
    static uint32_t   s_GlfwInitCount = 0;
    static std::mutex s_GlfwInitMutex;
    static std::mutex s_GlfwCreateMutex;

    static void GlfwErrorCallback(
        int         code,
        const char* description)
    {
        Log::Window().Error("GLFW Error: {0} ({1})", description, code);
    }

    Window::Window(
        const WindowDesc& windowDesc) :
        m_Title(windowDesc.Title)
    {
        {
            std::scoped_lock initLock(s_GlfwInitMutex);
            if (s_GlfwInitCount++ == 0)
            {
                GLFWallocator glfwAllocator{
                    .allocate = [](size_t size, void*)
                    { return mi_malloc(size); },
                    .reallocate = [](void* block, size_t size, void*)
                    { return mi_realloc(block, size); },
                    .deallocate = [](void* block, void*)
                    { mi_free(block); }
                };

                glfwSetErrorCallback(GlfwErrorCallback);
                glfwInitAllocator(&glfwAllocator);
                AME_LOG_ASSERT(Log::Window(), glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");
            }
        }

        GLFWmonitor*       monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode    = glfwGetVideoMode(monitor);

        {

            std::scoped_lock createLock(s_GlfwCreateMutex);

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
        glfwGetWindowSize(m_Handle, &m_WindowSize.x, &m_WindowSize.y);

        //

        glfwSetWindowSizeCallback(
            m_Handle,
            [](GLFWwindow* glfwWindow, int width, int height)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                if (width && height)
                {
                    window->OnWindowSizeChanged().Broadcast(*window, window->m_WindowSize);
                }
                window->m_WindowSize = Math::Size2I{ width, height };
            });

        glfwSetWindowCloseCallback(
            m_Handle,
            [](GLFWwindow* glfwWindow)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                window->OnWindowClosed().Broadcast(*window);
            });

        if (windowDesc.CustomTitleBar)
        {
            glfwSetTitlebarHitTestCallback(
                m_Handle,
                [](GLFWwindow* glfwWindow, int x, int y, int* hit)
                {
                    auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                    bool wasHit = false;
                    window->OnWindowTitleHitTest().Broadcast(*window, { x, y }, wasHit);
                    *hit = wasHit;
                });
        }

        glfwSetWindowIconifyCallback(
            m_Handle,
            [](GLFWwindow* glfwWindow, int iconified)
            {
                auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
                bool wasHit = false;
                window->OnWindowMinized().Broadcast(*window, iconified);
            });
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Handle);

        std::scoped_lock initLock(s_GlfwInitMutex);
        if (!--s_GlfwInitCount)
        {
            glfwTerminate();
        }
    }

    GLFWwindow* Window::GetHandle() const
    {
        return m_Handle;
    }

    void Window::Close()
    {
        glfwSetWindowShouldClose(m_Handle, GLFW_TRUE);
    }

    bool Window::IsRunning() const
    {
        return glfwWindowShouldClose(m_Handle) == GLFW_FALSE;
    }

    const String& Window::GetTitle() const
    {
        return m_Title;
    }

    void Window::SetTitle(
        String title)
    {
        m_Title = std::move(title);
        glfwSetWindowTitle(m_Handle, m_Title.c_str());
    }

    Math::Vector2I Window::GetPosition() const
    {
        int x, y;
        glfwGetWindowPos(m_Handle, &x, &y);
        return { x, y };
    }

    void Window::SetPosition(
        const Math::Vector2I& position)
    {
        glfwSetWindowPos(m_Handle, position.x, position.y);
    }

    void Window::SetFullscreen(
        bool state)
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
    }

    Math::Size2I Window::GetSize() const
    {
        return m_WindowSize;
    }

    bool Window::IsMinimized() const
    {
        return glfwGetWindowAttrib(m_Handle, GLFW_ICONIFIED) == GLFW_TRUE;
    }

    void Window::Minimize()
    {
        glfwIconifyWindow(m_Handle);
    }

    void Window::Restore()
    {
        glfwRestoreWindow(m_Handle);
    }

    bool Window::IsMaximized() const
    {
        return glfwGetWindowAttrib(m_Handle, GLFW_MAXIMIZED) == GLFW_TRUE;
    }

    void Window::Maximize()
    {
        glfwMaximizeWindow(m_Handle);
    }

    bool Window::IsFullScreen() const
    {
        return glfwGetWindowMonitor(m_Handle) != nullptr;
    }

    bool Window::IsVisible() const
    {
        return glfwGetWindowAttrib(m_Handle, GLFW_VISIBLE) == GLFW_TRUE &&
               !IsMinimized() &&
               IsRunning();
    }

    void Window::SetSize(
        const Math::Size2I& size)
    {
        m_WindowSize = size;
        glfwSetWindowSize(m_Handle, size.Width(), size.Height());
    }

    void Window::SetIcon(
        const std::byte*      iconData,
        const Math::Vector2I& size)
    {
        // Set icon for window
        GLFWimage glfwImage{
            .width  = size.x,
            .height = size.y,
            .pixels = const_cast<uint8_t*>(std::bit_cast<const uint8_t*>(iconData))
        };
        glfwSetWindowIcon(m_Handle, 1, &glfwImage);
    }

    void Window::SetVisible(
        bool show)
    {
        if (show)
        {
            glfwShowWindow(m_Handle);
        }
        else
        {
            glfwHideWindow(m_Handle);
        }
    }

    void Window::RequestFocus()
    {
        glfwRequestWindowAttention(m_Handle);
    }

    bool Window::HasFocus() const
    {
        return glfwGetWindowAttrib(m_Handle, GLFW_FOCUSED) == GLFW_TRUE;
    }

    void Window::ProcessEvents()
    {
        glfwPollEvents();
    }
} // namespace Ame::Windowing