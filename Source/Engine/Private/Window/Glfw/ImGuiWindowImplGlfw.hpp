#pragma once

#include <Window/ImGuiWindow.hpp>
#include <GLFW/glfw3.h>

namespace Ame::Window
{
    class ImGuiWindowImplGlfw : public BaseObject<IImGuiWindow>
    {
    public:
        using Base = BaseObject<IImGuiWindow>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_ImGuiWindow, Base);

    public:
        IMPLEMENT_INTERFACE_CTOR(ImGuiWindowImplGlfw, IWindow* window) : Base(counters), m_Window(window)
        {
        }

    public:
        void InitializeImGui(void* imguiContext) override;
        void InstallImGuiCallbacks(void* imguiContext) override;
        void NewFrameImGui(void* imguiContext) override;
        void UninstallImGuiCallbacks(void* imguiContext) override;
        void ShutdownImGui(void* imguiContext) override;

    private:
        IWindow* m_Window = nullptr;
    };
} // namespace Ame::Window
