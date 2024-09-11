#include <Interfaces/Graphics/RendererImpl.hpp>

#include <Rhi/ImGui/ImGuiFrameContext.hpp>
#include <imgui.h>

namespace Ame::Interfaces
{
    RendererImpl::RendererImpl(IReferenceCounters* counters, Rhi::IRhiDevice* rhiDevice, Ecs::World* world,
                               Rhi::IImGuiRenderer* imguiRenderer) :
        Base(counters),
        m_RhiDevice(rhiDevice), m_CommonRenderPass(rhiDevice->GetCommonRenderPass()), m_World(world),
#ifndef AME_NO_IMGUI
        m_ImGuiRenderer(imguiRenderer),
#endif
        m_EntityCompositor(rhiDevice, world)
    {
        CreateCameraQuery();
    }

    bool RendererImpl::Tick()
    {
        if (m_RhiDevice->BeginFrame()) [[likely]]
        {
            ClearRenderTarget();

            OnRenderBegin.Invoke();
            OnRenderUpdate.Invoke();

            RunRenderGraph();

            OnRenderPostUpdate.Invoke();
            OnRenderEnd.Invoke();

#ifndef AME_NO_IMGUI
            {
                Rhi::ImGuiFrameContext imguiContext(m_ImGuiRenderer);
                OnImGuiRender.Invoke();
                OnImGuiPostRender.Invoke();
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

    void RendererImpl::ClearRenderTarget()
    {
        auto swapchain        = m_RhiDevice->GetSwapchain();
        auto immediateContext = m_RhiDevice->GetImmediateContext();

        Dg::ITextureView* renderTarget = swapchain->GetCurrentBackBufferRTV();
        Dg::ITextureView* depthStencil = swapchain->GetDepthBufferDSV();

        immediateContext->SetRenderTargets(
            1, &renderTarget, depthStencil, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        immediateContext->ClearRenderTarget(
            renderTarget, m_ClearColor.data(), Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        if (depthStencil)
        {
            immediateContext->ClearDepthStencil(depthStencil, Dg::CLEAR_DEPTH_FLAG | Dg::CLEAR_STENCIL_FLAG, 1.0f, 0,
                                                Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }
    }
} // namespace Ame::Interfaces