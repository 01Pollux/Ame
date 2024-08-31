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
        ImGuiWindowImplGlfw(IReferenceCounters* referenceCounters, GLFWwindow* handle) :
            Base(referenceCounters), m_Handle(handle)
        {
        }

    public:
        void InitializeImGui(void* imguiContext) override;
        void InstallImGuiCallbacks(void* imguiContext) override;
        void NewFrameImGui(void* imguiContext) override;
        void UninstallImGuiCallbacks(void* imguiContext) override;
        void ShutdownImGui(void* imguiContext) override;

    private:
        GLFWwindow* m_Handle = nullptr;
    };
} // namespace Ame::Window
