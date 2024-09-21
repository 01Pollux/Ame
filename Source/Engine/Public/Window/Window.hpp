#pragma once

#include <Core/Interface.hpp>
#include <Core/String.hpp>

#include <Window/WindowCreateDesc.hpp>

struct GLFWwindow;

namespace Ame::Window
{
    // {D04401D7-E3E3-45D8-B66D-FA5E05750D90}
    inline constexpr UId IID_Window{ 0xd04401d7, 0xe3e3, 0x45d8, { 0xb6, 0x6d, 0xfa, 0x5e, 0x5, 0x75, 0xd, 0x90 } };

    class WindowEventListener;
    class IGlfwDriver;

    class IWindow : public IObjectWithCallback
    {
    public:
        using IObjectWithCallback::IObjectWithCallback;

        [[nodiscard]] virtual bool IsRunning() const = 0;
        virtual void               Close()           = 0;

        virtual void SetTitle(const String& title)               = 0;
        virtual void SetSize(const Math::Size2I& size)           = 0;
        virtual void SetPosition(const Math::Vector2I& position) = 0;

        [[nodiscard]] virtual const String&       GetTitle() const    = 0;
        [[nodiscard]] virtual const Math::Size2I& GetSize() const     = 0;
        [[nodiscard]] virtual Math::Vector2I      GetPosition() const = 0;

        [[nodiscard]] virtual WindowEventListener&   GetEventListener()    = 0;
        [[nodiscard]] virtual SharedPtr<IGlfwDriver> GetGlfwDriver() const = 0;

    public:
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
    };

    [[nodiscard]] AME_ENGINE_API Ptr<IWindow> CreateWindow(const WindowType&       windowType,
                                                           const WindowCreateDesc& createDesc);
} // namespace Ame::Window
