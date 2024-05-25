#include <Gfx/RG/Pipelines/DeferredPlusPipeline.hpp>
#include <Gfx/RG/Graph.hpp>

#include <Gfx/RG/Passes/EntityCollectPass.hpp>
#include <Gfx/RG/Passes/GBufferPass.hpp>

namespace Ame::Gfx::RG
{
    DeferredPlusPipeline::DeferredPlusPipeline(
        Gfx::RG::Graph&              renderGraph,
        Ecs::Universe&               universe,
        Cache::CommonPipelineState&  commonPipelines,
        Cache::CommonShader&         commonShaders,
        Cache::MaterialBindingCache& materialCache)
    {
        auto& passStorage = renderGraph.GetPassStorage();

        passStorage.Clear();
        passStorage.NewPass<Std::EntityCollectPass>(
            universe,
            commonPipelines);
        passStorage.NewPass<Std::GBufferPass>(
            commonShaders,
            materialCache);
    }
} // namespace Ame::Gfx::RG