#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <Rhi/Device/Device.hpp>
#include <Rhi/Staging/DeferredStagingManager.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        EngineFrame&                          engineFrame,
        FrameTimer&                           frameTimer,
        Rhi::Device&                          rhiDevice,
        Rhi::Staging::DeferredStagingManager& stagingManager,
        Ecs::Universe&                        universe,
        RG::Graph&                            renderGraph,
        EntityCompositor&                     entityCompositor,
        Cache::CommonRenderPass&              commonRenderPass) :
        m_Frame(engineFrame),
        m_Timer(frameTimer),
        m_Device(rhiDevice),
        m_Universe(universe),
        m_StagingManager(stagingManager),
        m_Graph(renderGraph),
        m_EntityCompositor(entityCompositor),
        m_CommonRenderPass(commonRenderPass)
    {
        if (!rhiDevice.IsHeadless())
        {
            using namespace std::placeholders;

            m_OnWorldChange = universe.OnWorldChange({ &Renderer::OnWorldChange, this, _1 });
            m_OnStartFrame  = engineFrame.OnStartFrame({ &Renderer::OnStartFrame, this });
            m_OnUpdate      = engineFrame.OnUpdate({ &Renderer::OnUpdate, this });
            m_OnRender      = engineFrame.OnRender({ &Renderer::OnRender, this });
            m_OnEndFrame    = engineFrame.OnEndFrame({ &Renderer::OnEndFrame, this });
        }
    }

    //

    void Renderer::FlushDeferredUploads()
    {
        m_StagingManager.get().Flush();
    }
} // namespace Ame::Gfx