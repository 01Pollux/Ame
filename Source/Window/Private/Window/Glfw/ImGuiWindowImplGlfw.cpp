#include <Window/Window.hpp>
#include <Window/Glfw/ImGuiWindowImplGlfw.hpp>
#include <Window/GlfwDriver.hpp>

#ifndef AME_NO_IMGUI
#include <Window/Glfw/ImGuiGlfwImpl.hpp>
#include <Window/Glfw/ImGuiWindowImplGlfw.hpp>
#endif

#include <Log/Logger.hpp>

namespace Ame::Window
{
    void ImGuiWindowImplGlfw::InitializeImGui(void* imguiContext)
    {
#ifndef AME_NO_IMGUI
        m_Window->GetGlfwDriver()
            ->submit(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        AME_LOG_WARNING("ImGui context mismatch");
                    }
                    IMGUI_CHECKVERSION();

                    ImGuiIO& io = ImGui::GetIO();
                    io.ConfigFlags |= /*ImGuiConfigFlags_ViewportsEnable |*/ // TODO
                                      ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard |
                                      ImGuiConfigFlags_NavEnableGamepad;

                    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports | ImGuiBackendFlags_RendererHasVtxOffset;

                    ImGui_ImplGlfw_InitForOther(
                        std::bit_cast<ImGuiContext*>(imguiContext), m_Window->GetGlfwHandle(), false);
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::InstallImGuiCallbacks(void* imguiContext)
    {
#ifndef AME_NO_IMGUI
        m_Window->GetGlfwDriver()
            ->submit(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        AME_LOG_WARNING("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_InstallCallbacks(
                        std::bit_cast<ImGuiContext*>(imguiContext), m_Window->GetGlfwHandle());
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::NewFrameImGui(void* imguiContext)
    {
#ifndef AME_NO_IMGUI
        m_Window->GetGlfwDriver()
            ->submit(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        AME_LOG_WARNING("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_NewFrame(std::bit_cast<ImGuiContext*>(imguiContext), m_Window->GetGlfwHandle());
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::UninstallImGuiCallbacks(void* imguiContext)
    {
#ifndef AME_NO_IMGUI
        m_Window->GetGlfwDriver()
            ->submit(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        AME_LOG_WARNING("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_RestoreCallbacks(
                        std::bit_cast<ImGuiContext*>(imguiContext), m_Window->GetGlfwHandle());
                })
            .wait();
#endif
    }

    void ImGuiWindowImplGlfw::ShutdownImGui(void* imguiContext)
    {
#ifndef AME_NO_IMGUI
        m_Window->GetGlfwDriver()
            ->submit(
                [this, imguiContext]
                {
                    if (imguiContext != ImGui::GetCurrentContext())
                    {
                        AME_LOG_WARNING("ImGui context mismatch");
                    }
                    ImGui_ImplGlfw_Shutdown(std::bit_cast<ImGuiContext*>(imguiContext), m_Window->GetGlfwHandle());
                })
            .wait();
#endif
    }
} // namespace Ame::Window