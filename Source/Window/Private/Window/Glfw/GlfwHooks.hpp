#pragma once

#include <functional>
#include <map>

#include <GLFW/glfw3.h>

namespace Ame::Window
{
    /// <summary>
    /// Since glfw is a singleton, we need to have a way to properly manage and dispatch callbacks between the hooks.
    /// All operations must be called inside GlfwContext
    /// </summary>
    class GlfwHooks
    {
    public:
        static constexpr uint32_t InvalidId = 0;

    public:
        /// <summary>
        /// Get the glfw hooks
        /// </summary>
        [[nodiscard]] static GlfwHooks& Get();

#define AME_INSTALL_GLFW_CALLBACKS_INSTALLER_IMPL(name)                                                                \
private:                                                                                                               \
    template<typename... Args> static void GLFW##name##Callback(Args... args)                                          \
    {                                                                                                                  \
        auto& self = Get();                                                                                            \
        for (auto& [id, callback] : self.m_GLFW##name##Callbacks)                                                      \
        {                                                                                                              \
            if (!callback(std::forward<Args>(args)...))                                                                \
            {                                                                                                          \
                return;                                                                                                \
            }                                                                                                          \
        }                                                                                                              \
        if (self.m_Old##name##Callback)                                                                                \
        {                                                                                                              \
            self.m_Old##name##Callback(std::forward<Args>(args)...);                                                   \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    template<typename FnTy> [[nodiscard]] uint32_t Install_##name(FnTy&& callback)                                     \
    {                                                                                                                  \
        auto id = m_IdCounter++;                                                                                       \
        m_GLFW##name##Callbacks.emplace(id, std::forward<FnTy>(callback));                                             \
        Install_##name##Once();                                                                                        \
        return id;                                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    void Uninstall_##name(uint32_t id)                                                                                 \
    {                                                                                                                  \
        m_GLFW##name##Callbacks.erase(id);                                                                             \
    }

#define AME_INSTALL_GLFW_CALLBACKS(callbackType, setName, name, ...)                                                   \
private:                                                                                                               \
    void Install_##name##Once()                                                                                        \
    {                                                                                                                  \
        auto& self = Get();                                                                                            \
        if (self.m_Old##name##Callback == nullptr)                                                                     \
        {                                                                                                              \
            self.m_Old##name##Callback = setName(&GLFW##name##Callback<__VA_ARGS__>);                                  \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    AME_INSTALL_GLFW_CALLBACKS_INSTALLER_IMPL(name);                                                                   \
                                                                                                                       \
private:                                                                                                               \
    using name##callbackType = bool(__VA_ARGS__);                                                                      \
    using name##callbackMap  = std::map<uint32_t, std::move_only_function<name##callbackType>>;                        \
                                                                                                                       \
    name##callbackMap m_GLFW##name##Callbacks;                                                                         \
    callbackType      m_Old##name##Callback = nullptr

#define AME_INSTALL_GLFW_CALLBACKS_WINDOW_INSTALLER(name)                                                              \
private:                                                                                                               \
    template<typename... Args> static void GLFW##name##Callback(GLFWwindow* window, Args... args)                      \
    {                                                                                                                  \
        auto& self                          = Get();                                                                   \
        auto& [originalCallback, listeners] = self.m_##name##Callbacks[window];                                        \
        for (auto& [id, callback] : listeners)                                                                         \
        {                                                                                                              \
            if (!callback(window, std::forward<Args>(args)...))                                                        \
            {                                                                                                          \
                return;                                                                                                \
            }                                                                                                          \
        }                                                                                                              \
        if (originalCallback)                                                                                          \
        {                                                                                                              \
            originalCallback(window, std::forward<Args>(args)...);                                                     \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    template<typename FnTy> [[nodiscard]] uint32_t Install_##name(GLFWwindow* window, FnTy&& callback)                 \
    {                                                                                                                  \
        auto id = m_IdCounter++;                                                                                       \
        m_##name##Callbacks[window].Listeners.emplace(id, std::forward<FnTy>(callback));                               \
        Install_##name##Once(window);                                                                                  \
        return id;                                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    void Uninstall_##name(GLFWwindow* window, uint32_t id)                                                             \
    {                                                                                                                  \
        m_##name##Callbacks[window].Listeners.erase(id);                                                               \
    }

#define AME_INSTALL_GLFW_CALLBACKS_WINDOW(callbackType, setName, name, ...)                                            \
private:                                                                                                               \
    void Install_##name##Once(GLFWwindow* window)                                                                      \
    {                                                                                                                  \
        auto& self     = Get();                                                                                        \
        auto& callback = self.m_##name##Callbacks[window].Callback;                                                    \
        if (callback == nullptr)                                                                                       \
        {                                                                                                              \
            callback = setName(window, &GLFW##name##Callback<__VA_ARGS__>);                                            \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
public:                                                                                                                \
    AME_INSTALL_GLFW_CALLBACKS_WINDOW_INSTALLER(name);                                                                 \
                                                                                                                       \
private:                                                                                                               \
    using name##callbackType = bool(GLFWwindow*, __VA_ARGS__);                                                         \
    using name##callbackMap  = std::map<uint32_t, std::move_only_function<name##callbackType>>;                        \
    struct name##OldAndCallbacks                                                                                       \
    {                                                                                                                  \
        callbackType      Callback;                                                                                    \
        name##callbackMap Listeners;                                                                                   \
    };                                                                                                                 \
    using callbackType##Map = std::map<GLFWwindow*, name##OldAndCallbacks>;                                            \
                                                                                                                       \
    callbackType##Map m_##name##Callbacks

    public:
        AME_INSTALL_GLFW_CALLBACKS(GLFWerrorfun, glfwSetErrorCallback, Error, int, const char*);
        AME_INSTALL_GLFW_CALLBACKS(GLFWmonitorfun, glfwSetMonitorCallback, Monitor, GLFWmonitor*, int);

        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowposfun, glfwSetWindowPosCallback, WindowPos, int, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWtitlebarhittestfun, glfwSetTitlebarHitTestCallback, TitlebarHitTest, int,
                                          int, int*);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowsizefun, glfwSetWindowSizeCallback, WindowSize, int, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowclosefun, glfwSetWindowCloseCallback, WindowClose);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowrefreshfun, glfwSetWindowRefreshCallback, WindowRefresh);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowfocusfun, glfwSetWindowFocusCallback, WindowFocus, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowiconifyfun, glfwSetWindowIconifyCallback, WindowIconified, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowmaximizefun, glfwSetWindowMaximizeCallback, WindowMaximized, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWframebuffersizefun, glfwSetFramebufferSizeCallback, WindowFrameBufferSize,
                                          int, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWwindowcontentscalefun, glfwSetWindowContentScaleCallback,
                                          WindowContentScale, float, float);

        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWkeyfun, glfwSetKeyCallback, Key, int, int, int, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWcharfun, glfwSetCharCallback, Char, unsigned int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWcharmodsfun, glfwSetCharModsCallback, CharMods, unsigned int, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWmousebuttonfun, glfwSetMouseButtonCallback, MouseButton, int, int, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWcursorposfun, glfwSetCursorPosCallback, CursorPos, double, double);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWcursorenterfun, glfwSetCursorEnterCallback, CursorEnter, int);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWscrollfun, glfwSetScrollCallback, Scroll, double, double);
        AME_INSTALL_GLFW_CALLBACKS_WINDOW(GLFWdropfun, glfwSetDropCallback, DropPath, int, const char**);

        AME_INSTALL_GLFW_CALLBACKS(GLFWjoystickfun, glfwSetJoystickCallback, Joystick, int, int);

    private:
#undef AME_INSTALL_GLFW_CALLBACKS_IMPL
#undef AME_INSTALL_GLFW_CALLBACKS_INSTALLER
#undef AME_INSTALL_GLFW_CALLBACKS
#undef AME_INSTALL_GLFW_CALLBACKS_WINDOW
#undef AME_INSTALL_GLFW_CALLBACKS_WINDOW_INSTALLER

    private:
        static inline uint32_t m_IdCounter = 1;
    }; // namespace Ame::Window
} // namespace Ame::Window