#pragma once

#include <Gfx/RG/Pipelines/BaseGraphPipeline.hpp>

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

namespace Ame::Gfx::RG
{
    class DeferredPlusPipeline final : public IBaseGraphPipeline
    {
    public:
        DeferredPlusPipeline(
            Gfx::RG::Graph&              renderGraph,
            Ecs::Universe&               universe,
            Cache::CommonPipelineState&  commonPipelines,
            Cache::CommonShader&         commonShaders,
            Cache::MaterialBindingCache& materialCache);
    };
} // namespace Ame::Gfx::RG