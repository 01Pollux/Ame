#pragma once

#include <Gfx/RenderGraph/Pipelines/BaseGraphPipeline.hpp>

#include <Gfx/RenderGraph/Compositors/SpriteBatchCompositor.hpp>

namespace Ame
{
    namespace Ecs
    {
        class Universe;
    } // namespace Ecs
    namespace Gfx::Cache
    {
        class CommonShader;
        class CommonPipelineState;
        class MaterialBindingCache;
    } // namespace Gfx::Cache
} // namespace Ame

namespace Ame::Gfx
{
    class ForwardPlusPipeline final : public IBaseGraphPipeline
    {
    public:
        ForwardPlusPipeline(
            RG::Graph&                   renderGraph,
            Ecs::Universe&               universe,
            EntityCompositor&            entityCompositor,
            Cache::CommonShader&         commonShaders,
            Cache::CommonPipelineState&  commonPipelines,
            Cache::MaterialBindingCache& materialCache);

    private:
        SpriteBatchCompositor m_SpriteBatchCompositor;
    };
} // namespace Ame::Gfx