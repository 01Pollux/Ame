#include <FlappyRocket/Engine.hpp>

#include <Gfx/Renderer.hpp>
#include <Gfx/RG/Passes/EntityCollectPass.hpp>
#include <Gfx/RG/Passes/GBufferPass.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    namespace RG = Gfx::RG;

    void FlappyRocketGame::SetupRenderGraph(
        Gfx::RG::Graph&                   RenderGraph,
        Gfx::Cache::CommonPipelineState&  CommonPipelines,
        Gfx::Cache::MaterialBindingCache& MaterialCache)
    {
        auto& PassStorage = RenderGraph.GetPassStorage();

        PassStorage.NewPass<RG::Std::EntityCollectPass>(
            *m_EcsUniverse,
            CommonPipelines);
        PassStorage.NewPass<RG::Std::GBufferPass>(
            MaterialCache);
    }
} // namespace Ame::FlappyRocket
