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
        int         Code,
        const char* Description)
    {
        Log::Window().Error("GLFW Error: {0} ({1})", Description, Code);
    }

    Window::Window(
        const WindowDesc& Desc) :
        m_Title(Desc.Title)
    {
        {
            std::scoped_lock Lock(s_GlfwInitMutex);
            if (s_GlfwInitCount++ == 0)
            {
                GLFWallocator GlfwAllocator{
                    .allocate = [](size_t Size, void*)
                    { return mi_malloc(Size); },
                    .reallocate = [](void* Block, size_t Size, void*)
                    { return mi_realloc(Block, Size); },
                    .deallocate = [](void* Block, void*)
                    { mi_free(Block); }
                };

                glfwSetErrorCallback(GlfwErrorCallback);
                glfwInitAllocator(&GlfwAllocator);
                AME_LOG_ASSERT(Log::Window(), glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");
            }
        }

        GLFWmonitor*       Monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* Mode    = glfwGetVideoMode(Monitor);

        {

            std::scoped_lock Lock(s_GlfwCreateMutex);

            glfwDefaultWindowHints();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#ifdef AME_PLATFORM_WINDOWS
            glfwWindowHint(GLFW_TITLEBAR, !Desc.CustomTitleBar);
#else
            glfwWindowHint(GLFW_DECORATED, !Desc.CustomTitleBar);
#endif

            if (Desc.FullScreen)
            {
                m_Handle = glfwCreateWindow(Mode->width, Mode->height, Desc.Title, Monitor, nullptr);
            }
            else
            {
                m_Handle = glfwCreateWindow(Desc.Size.Width(), Desc.Size.Height(), Desc.Title, nullptr, nullptr);
            }
        }

        if (Desc.FullScreen)
        {
            if (Desc.StartInMiddle)
            {
                int X = (Mode->width - Desc.Size.Width()) / 2;
                int Y = (Mode->height - Desc.Size.Height()) / 2;

                glfwSetWindowPos(m_Handle, X, Y);
            }

            if (Desc.Maximized)
            {
                glfwMaximizeWindow(m_Handle);
            }
        }

        if (Desc.NoResize)
        {
            glfwSetWindowAttrib(m_Handle, GLFW_RESIZABLE, GLFW_FALSE);
        }

        glfwSetWindowUserPointer(m_Handle, this);
        glfwGetWindowSize(m_Handle, &m_WindowSize.x, &m_WindowSize.y);

        //

        glfwSetWindowSizeCallback(
            m_Handle,
            [](GLFWwindow* GlfwWindow, int Width, int Height)
            {
                auto App = static_cast<Window*>(glfwGetWindowUserPointer(GlfwWindow));

                if (Width && Height)
                {
                    App->OnWindowSizeChanged().Broadcast(*App, App->m_WindowSize);
                }
                App->m_WindowSize = Math::Size2I{ Width, Height };
            });

        glfwSetWindowCloseCallback(
            m_Handle,
            [](GLFWwindow* GlfwWindow)
            {
                auto App = static_cast<Window*>(glfwGetWindowUserPointer(GlfwWindow));
                App->OnWindowClosed().Broadcast(*App);
            });

        if (Desc.CustomTitleBar)
        {
            glfwSetTitlebarHitTestCallback(
                m_Handle,
                [](GLFWwindow* GlfwWindow, int X, int Y, int* Hit)
                {
                    auto App    = static_cast<Window*>(glfwGetWindowUserPointer(GlfwWindow));
                    bool WasHit = false;
                    App->OnWindowTitleHitTest().Broadcast(*App, { X, Y }, WasHit);
                    *Hit = WasHit;
                });
        }

        glfwSetWindowIconifyCallback(
            m_Handle,
            [](GLFWwindow* GlfwWindow, int Iconified)
            {
                auto App    = static_cast<Window*>(glfwGetWindowUserPointer(GlfwWindow));
                bool WasHit = false;
                App->OnWindowMinized().Broadcast(*App, Iconified);
            });
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Handle);

        std::scoped_lock Lock(s_GlfwInitMutex);
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
        String Title)
    {
        m_Title = std::move(Title);
        glfwSetWindowTitle(m_Handle, m_Title.c_str());
    }

    Math::Vector2I Window::GetPosition() const
    {
        int X, Y;
        glfwGetWindowPos(m_Handle, &X, &Y);
        return { X, Y };
    }

    void Window::SetPosition(
        const Math::Vector2I& Position)
    {
        glfwSetWindowPos(m_Handle, Position.x, Position.y);
    }

    void Window::SetFullscreen(
        bool State)
    {
        if (State)
        {
            GLFWmonitor*       Monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* Mode    = glfwGetVideoMode(Monitor);

            m_WindowSize = { Mode->width, Mode->height };
            glfwSetWindowMonitor(m_Handle, Monitor, 0, 0, Mode->width, Mode->height, Mode->refreshRate);
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
        const Math::Size2I& Size)
    {
        m_WindowSize = Size;
        glfwSetWindowSize(m_Handle, Size.Width(), Size.Height());
    }

    void Window::SetIcon(
        void*                 IconData,
        const Math::Vector2I& Size)
    {
        // Set icon for window
        GLFWimage Image{
            .width  = Size.x,
            .height = Size.y,
            .pixels = const_cast<uint8_t*>(static_cast<const uint8_t*>(IconData))
        };
        glfwSetWindowIcon(m_Handle, 1, &Image);
    }

    void Window::SetVisible(
        bool Show)
    {
        if (Show)
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