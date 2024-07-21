#include <Window/Window.hpp>
#include <Window/Glfw/ImGuiWindowImplGlfw.hpp>
#include <Window/Glfw/GlfwContext.hpp>

#ifndef AME_DIST
#include <Window/Glfw/ImGuiGlfwImpl.hpp>
#include <Window/Glfw/ImGuiWindowImplGlfw.hpp>
#endif

#include <Log/Wrapper.hpp>

namespace Ame::Window
{
    void ImGuiWindowImplGlfw::InitializeImGui(
        void* imguiContext)
    {
#ifndef AME_DIST
        GlfwContext::Get()
            .PushTask(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        Log::Window().Warning("ImGui context mismatch");
                    }
                    IMGUI_CHECKVERSION();

                    ImGuiIO& io = ImGui::GetIO();
                    io.BackendFlags |= ImGuiConfigFlags_NavEnableKeyboard |
                                       ImGuiConfigFlags_NavEnableGamepad |
                                       ImGuiConfigFlags_DockingEnable |
                                       ImGuiConfigFlags_ViewportsEnable;

                    ImGui_ImplGlfw_InitForOther(std::bit_cast<ImGuiContext*>(imguiContext), m_Handle, false);
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::InstallImGuiCallbacks(
        void* imguiContext)
    {
#ifndef AME_DIST
        GlfwContext::Get()
            .PushTask(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        Log::Window().Warning("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_InstallCallbacks(std::bit_cast<ImGuiContext*>(imguiContext), m_Handle);
                    ImGui_ImplGlfw_SetCallbacksChainForAllWindows(std::bit_cast<ImGuiContext*>(imguiContext), m_Handle, true);
                    glfwPollEvents();
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::NewFrameImGui(
        void* imguiContext)
    {
#ifndef AME_DIST
        GlfwContext::Get()
            .PushTask(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        Log::Window().Warning("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_NewFrame(std::bit_cast<ImGuiContext*>(imguiContext), m_Handle);
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::UninstallImGuiCallbacks(
        void* imguiContext)
    {
#ifndef AME_DIST
        GlfwContext::Get()
            .PushTask(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        Log::Window().Warning("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_RestoreCallbacks(std::bit_cast<ImGuiContext*>(imguiContext), m_Handle);
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::ShutdownImGui(
        void* imguiContext)
    {
#ifndef AME_DIST
        GlfwContext::Get()
            .PushTask(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        Log::Window().Warning("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_Shutdown(std::bit_cast<ImGuiContext*>(imguiContext), m_Handle);
                })
            .wait();
#endif
    }
} // namespace Ame::Window