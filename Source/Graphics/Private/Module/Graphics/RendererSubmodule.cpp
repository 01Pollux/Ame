#include <Module/Graphics/RendererSubmodule.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Rhi/Device/RhiDevice.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <Rhi/ImGui/ImGuiFrameContext.hpp>

#include <imgui.h>

namespace Ame
{
    RendererSubmodule::RendererSubmodule(
        IReferenceCounters* counters,
        RhiModule*          rhiModule,
        uint32_t            syncInterval) :
        Base(counters, IID_RendererSubmodule),
        m_SyncInterval(syncInterval)
    {
        rhiModule->QueryInterface(Rhi::IID_RhiDevice, m_RhiDevice.DblPtr<IObject>());
        rhiModule->QueryInterface(Dg::IID_DeviceContext, m_DeviceContext.DblPtr<IObject>());
        rhiModule->QueryInterface(Dg::IID_SwapChain, m_Swapchain.DblPtr<IObject>());
#ifndef AME_DIST
        rhiModule->QueryInterface(Rhi::IID_ImGuiRenderer, m_ImGuiRenderer.DblPtr<IObject>());
#endif
    }

    RendererSubmodule::~RendererSubmodule() = default;

    bool RendererSubmodule::Tick()
    {
        if (m_RhiDevice->BeginFrame()) [[likely]]
        {
            ClearRenderTarget();

            Invoke_OnRenderBegin();
            Invoke_OnRenderUpdate();
            Invoke_OnRenderPostUpdate();
            Invoke_OnRenderEnd();

#ifndef AME_DIST
            {
                Rhi::ImGuiFrameContext imguiContext(m_ImGuiRenderer);
                Invoke_OnImGuiRender();
                Invoke_OnImGuiPostRender();

                ImGui::ShowDemoWindow();
                // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
                {
                    static float f       = 0.0f;
                    static int   counter = 0;

                    ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

                    ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

                    ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

                    if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                        counter++;
                    ImGui::SameLine();
                    ImGui::Text("counter = %d", counter);

                    auto& io = ImGui::GetIO();
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                    ImGui::End();
                }
            }
#endif
            m_RhiDevice->AdvanceFrame(m_SyncInterval);
            return true;
        }
        else
        {
            return false;
        }
    }

    void RendererSubmodule::ClearRenderTarget()
    {
        Dg::ITextureView* renderTarget = m_Swapchain->GetCurrentBackBufferRTV();
        Dg::ITextureView* depthStencil = m_Swapchain->GetDepthBufferDSV();

        m_DeviceContext->SetRenderTargets(1, &renderTarget, depthStencil, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_DeviceContext->ClearRenderTarget(renderTarget, m_ClearColor.data(), Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        if (depthStencil)
        {
            m_DeviceContext->ClearDepthStencil(depthStencil, Dg::CLEAR_DEPTH_FLAG | Dg::CLEAR_STENCIL_FLAG, 1.0f, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }
    }
} // namespace Ame