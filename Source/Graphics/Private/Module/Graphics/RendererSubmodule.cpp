#include <Module/Graphics/RendererSubmodule.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Rhi/Device/RhiDevice.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <Rhi/ImGui/ImGuiFrameContext.hpp>

#include <imgui.h>

namespace Ame
{
    RendererSubmodule::RendererSubmodule(
        IReferenceCounters*         counters,
        RhiModule*                  rhiModule,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_RendererSubmodule),
        m_SyncInterval(config.SyncInterval)
    {
        rhiModule->QueryInterface(Rhi::IID_RhiDevice, m_RhiDevice.DblPtr<IObject>());
        rhiModule->QueryInterface(Dg::IID_DeviceContext, m_DeviceContext.DblPtr<IObject>());
        rhiModule->QueryInterface(Dg::IID_SwapChain, m_Swapchain.DblPtr<IObject>());
#ifndef AME_NO_IMGUI
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

#ifndef AME_NO_IMGUI
            {
                Rhi::ImGuiFrameContext imguiContext(m_ImGuiRenderer);
                Invoke_OnImGuiRender();
                Invoke_OnImGuiPostRender();
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