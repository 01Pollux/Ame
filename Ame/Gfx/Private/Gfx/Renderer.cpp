#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        EngineFrame&             engineFrame,
        FrameTimer&              frameTimer,
        Rhi::RhiDevice&          rhiDevice,
        Ecs::Universe&           universe,
        RG::Graph&               renderGraph,
        EntityCompositor&        entityCompositor,
        Cache::CommonRenderPass& commonRenderPass) :
        m_Frame(engineFrame),
        m_Timer(frameTimer),
        m_Device(rhiDevice),
        m_Universe(universe),
        m_Graph(renderGraph),
        m_EntityCompositor(entityCompositor),
        m_CommonRenderPass(commonRenderPass)
    {
        if (!rhiDevice.IsHeadless())
        {
            using namespace std::placeholders;

            m_OnWorldChange = universe.OnWorldChange({ &Renderer::OnWorldChange, this, _1 });
        }

        CreateEmptyGraph();
    }

    Co::result<void> Renderer::Tick(
        Co::runtime&)
    {
        bool isRunning = m_Device.get().ProcessTasks();
        
        BuildRenderGraph();
        RunRenderGraph();

        m_Device.get().ProcessTasks();

        if (!isRunning)
        {
            m_Frame.get().Stop();
        }
    }
} // namespace Ame::Gfx