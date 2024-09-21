#pragma once

#include <Window/Window.hpp>
#include <Window/WindowEventListener.hpp>
#include <GLFW/glfw3.h>

namespace Ame::Window
{
    struct WindowCreateDesc;

    class DesktopWindowImplGlfw : public IWindow
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_Window, IWindow);

    private:
        IMPLEMENT_INTERFACE_CTOR(DesktopWindowImplGlfw, const WindowCreateDesc& windowDesc);

    public:
        ~DesktopWindowImplGlfw() override;

    public:
        bool IsRunning() const override;
        void Close() override;

        void SetTitle(const String& title) override;
        void SetSize(const Math::Size2I& size) override;
        void SetPosition(const Math::Vector2I& position) override;

        const String&       GetTitle() const override;
        const Math::Size2I& GetSize() const override;
        Math::Vector2I      GetPosition() const override;

    public:
        GLFWwindow* GetGlfwHandle() const override;

        void SetFullscreen(bool state) override;

        bool IsMinimized() const override;
        bool IsMaximized() const override;

        void Maximize() override;
        void Minimize() override;

        bool IsFullScreen() const override;
        bool IsVisible() const override;

        bool HasFocus() const override;
        void RequestFocus() override;
        void SetVisible(bool show) override;

        void Restore() override;

        [[nodiscard]] WindowEventListener&   GetEventListener() override;
        [[nodiscard]] SharedPtr<IGlfwDriver> GetGlfwDriver() const override;

    private:
        void CreateGlfwWindow(const WindowCreateDesc& windowDesc);

    private:
        GLFWwindow*  m_Handle = nullptr;
        String       m_Title;
        Math::Size2I m_WindowSize;

        WindowEventListener m_EventListener;

        uint32_t m_WindowSizeCallbackId     = 0;
        uint32_t m_WindowPosCallbackId      = 0;
        uint32_t m_WindowTitlebarCallbackId = 0;
        uint32_t m_WindowIconifyCallbackId  = 0;
    };
} // namespace Ame::Window
