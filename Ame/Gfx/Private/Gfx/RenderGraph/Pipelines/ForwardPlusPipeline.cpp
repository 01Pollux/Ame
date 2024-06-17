#include <RG/Graph.hpp>
#include <Gfx/RenderGraph/Pipelines/ForwardPlusPipeline.hpp>

#include <Gfx/RenderGraph/Passes/ForwardOpaquePass.hpp>

namespace Ame::Gfx
{
    ForwardPlusPipeline::ForwardPlusPipeline(
        RG::Graph&                             renderGraph,
        Ecs::Universe&                         universe,
        EntityCompositor&                      entityCompositor,
        Cache::FrameDynamicBufferCache<false>& bufferCache,
        Cache::CommonShader&                   commonShaders,
        Cache::CommonPipelineState&            commonPipelines,
        Cache::MaterialBindingCache&           materialCache) :
        m_StandardBatchedCompositor(entityCompositor, bufferCache)
    {
        auto& passStorage = renderGraph.GetPassStorage();

        passStorage.Clear();
        passStorage.NewPass<ForwardOpaquePass>(
            "ForwardOpaquePass",
            entityCompositor,
            commonShaders,
            materialCache);
    }
} // namespace Ame::Gfx