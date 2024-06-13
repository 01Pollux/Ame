#include <RG/Graph.hpp>
#include <Gfx/RenderGraph/Pipelines/DeferredPlusPipeline.hpp>

#include <Gfx/RenderGraph/Passes/EntityCollectPass.hpp>
#include <Gfx/RenderGraph/Passes/GBufferPass.hpp>

namespace Ame::Gfx
{
    DeferredPlusPipeline::DeferredPlusPipeline(
        RG::Graph&                   renderGraph,
        Ecs::Universe&               universe,
        Cache::CommonShader&         commonShaders,
        Cache::CommonPipelineState&  commonPipelines,
        Cache::MaterialBindingCache& materialCache)
    {
        auto& passStorage = renderGraph.GetPassStorage();

        passStorage.Clear();
        passStorage.NewPass<EntityCollectPass>(
            "EntityCollectPass",
            universe,
            commonPipelines);
        passStorage.NewPass<GBufferPass>(
            "GBufferPass",
            commonShaders,
            materialCache);
    }
} // namespace Ame::Gfx