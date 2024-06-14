#include <RG/Graph.hpp>
#include <Gfx/RenderGraph/Pipelines/ForwardPlusPipeline.hpp>

#include <Gfx/RenderGraph/Passes/ForwardOpaquePass.hpp>

namespace Ame::Gfx
{
    ForwardPlusPipeline::ForwardPlusPipeline(
        RG::Graph&                   renderGraph,
        Ecs::Universe&               universe,
        Cache::CommonShader&         commonShaders,
        Cache::CommonPipelineState&  commonPipelines,
        Cache::MaterialBindingCache& materialCache)
    {
        auto& passStorage = renderGraph.GetPassStorage();

        passStorage.Clear();
        passStorage.NewPass<ForwardOpaquePass>(
            "ForwardOpaquePass",
            commonShaders,
            materialCache);
    }
} // namespace Ame::Gfx