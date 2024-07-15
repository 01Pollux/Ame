#pragma once

#include <Window/Window.hpp>

struct GLFWwindow;

namespace Ame::Window
{
    AME_DECL_CLASS(WindowGlfw, "{35e69e93-2904-4182-a85e-baf623c86752}");

    AME_INTERFACE_BEGIN_EX("{7fb8e691-1cfd-4c23-9981-930830e14162}", IDesktopWindow, public IWindow)

    [[nodiscard]] virtual GLFWwindow* GetGlfwHandle() const = 0;

    virtual void SetFullscreen(bool state) = 0;

    [[nodiscard]] virtual bool IsMinimized() const = 0;
    [[nodiscard]] virtual bool IsMaximized() const = 0;

    virtual void Maximize() = 0;
    virtual void Minimize() = 0;

    [[nodiscard]] virtual bool IsFullScreen() const = 0;
    [[nodiscard]] virtual bool IsVisible() const    = 0;

    [[nodiscard]] virtual bool HasFocus() const      = 0;
    virtual void               RequestFocus()        = 0;
    virtual void               SetVisible(bool show) = 0;

    /// <summary>
    /// This function restores the specified window if it was previously iconified
    ///  (minimized) or maximized.  If the window is already restored, this function
    ///  does nothing.
    /// </summary>
    virtual void Restore() = 0;

    AME_INTERFACE_END();
} // namespace Ame::Window
