#pragma once

#include <Window/Window.hpp>

struct GLFWwindow;

namespace Ame::Window
{
    // {CAAD6B48-505E-4B7A-A962-EA5FAC0718FD}
    static constexpr UId IID_DesktopWindow{ 0xcaad6b48, 0x505e, 0x4b7a, { 0xa9, 0x62, 0xea, 0x5f, 0xac, 0x7, 0x18, 0xfd } };

    class IDesktopWindow : public IWindow
    {
    public:
        [[nodiscard]] virtual GLFWwindow* AME_METHOD(GetGlfwHandle)() const = 0;

        virtual void AME_METHOD(SetFullscreen)(bool state) = 0;

        [[nodiscard]] virtual bool AME_METHOD(IsMinimized)() const = 0;
        [[nodiscard]] virtual bool AME_METHOD(IsMaximized)() const = 0;

        virtual void AME_METHOD(Maximize)() = 0;
        virtual void AME_METHOD(Minimize)() = 0;

        [[nodiscard]] virtual bool AME_METHOD(IsFullScreen)() const = 0;
        [[nodiscard]] virtual bool AME_METHOD(IsVisible)() const    = 0;

        [[nodiscard]] virtual bool AME_METHOD(HasFocus)() const      = 0;
        virtual void               AME_METHOD(RequestFocus)()        = 0;
        virtual void               AME_METHOD(SetVisible)(bool show) = 0;

        /// <summary>
        /// This function restores the specified window if it was previously iconified
        ///  (minimized) or maximized.  If the window is already restored, this function
        ///  does nothing.
        /// </summary>
        virtual void AME_METHOD(Restore)() = 0;
    };
} // namespace Ame::Window
