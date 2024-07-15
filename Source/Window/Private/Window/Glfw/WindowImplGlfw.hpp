#pragma once

#include <Window/DesktopWindow.hpp>
#include <Window/WindowEventListener.hpp>
#include <GLFW/glfw3.h>

namespace Ame::Window
{
    struct WindowCreateDesc;

    AME_MT_OBJECT(WindowImplGlfw)
    AME_INTERFACE_OF(IDesktopWindow)
    {
    public:
        AME_BEGIN_INTERFACE_MAP(WindowImplGlfw, CLSID_WindowGlfw)
        AME_INTERFACE_ENTRY(IDesktopWindow)
        AME_COMPONENT_ENTRY(m_EventListener)
        AME_END_INTERFACE_MAP()

    public:
        void Initialize(
            const WindowCreateDesc& windowDesc);

        ~WindowImplGlfw() override;

        PtError FinalConstruct();

    public:
        void ProcessEvents() override;
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

    private:
        void CreateGlfwWindow(const WindowCreateDesc& windowDesc);

    private:
        GLFWwindow*  m_Handle = nullptr;
        String       m_Title;
        Math::Size2I m_WindowSize;

        Ptr<WindowEventListener> m_EventListener;
    };

    AME_CLASS_REGISTER(WindowImplGlfw);
} // namespace Ame::Window
