#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        EngineFrame&             engineFrame,
        FrameTimer&              frameTimer,
        Rhi::Device&             rhiDevice,
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
        if (!m_Device.get().BeginFrame())
        {
            m_Frame.get().Stop();
            co_return;
        }

        BuildRenderGraph();
        m_Device.get().ProcessTasks();
        RunRenderGraph();

        m_Device.get().EndFrame();
        co_return;
    }
} // namespace Ame::Gfx