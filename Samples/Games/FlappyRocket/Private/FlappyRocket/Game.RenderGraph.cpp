#include <FlappyRocket/Engine.hpp>

#include <Gfx/Renderer.hpp>
#include <Gfx/RG/Passes/EntityCollectPass.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    namespace RG = Gfx::RG;

    void FlappyRocketGame::SetupRenderGraph(
        Gfx::RG::Graph&                 RenderGraph,
        Gfx::Cache::PipelineStateCache& PipelineStateCache)
    {
        auto& PassStorage = RenderGraph.GetPassStorage();

        PassStorage.NewPass<RG::Std::EntityCollectPass>(
            *m_EcsUniverse,
            PipelineStateCache);
    }
} // namespace Ame::FlappyRocket
