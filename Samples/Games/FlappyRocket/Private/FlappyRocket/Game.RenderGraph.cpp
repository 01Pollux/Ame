#include <FlappyRocket/Engine.hpp>

#include <Gfx/Renderer.hpp>
#include <Gfx/RG/Passes/EntityCollectPass.hpp>
#include <Gfx/RG/Passes/GBufferPass.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::FlappyRocket
{
    namespace RG = Gfx::RG;

    void FlappyRocketGame::SetupRenderGraph(
        Gfx::RG::Graph&                   renderGraph,
        Gfx::Cache::CommonShader&         commonShaders,
        Gfx::Cache::CommonPipelineState&  commonPipelines,
        Gfx::Cache::MaterialBindingCache& materialCache)
    {
        auto& passStorage = renderGraph.GetPassStorage();

        passStorage.NewPass<RG::Std::EntityCollectPass>(
            *m_EcsUniverse,
            commonPipelines);
        passStorage.NewPass<RG::Std::GBufferPass>(
            commonShaders,
            materialCache);
    }
} // namespace Ame::FlappyRocket
