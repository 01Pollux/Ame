#pragma once

#include <Gfx/RG/Pass.hpp>
#include <Ecs/Universe.hpp>

namespace Ame::Gfx::Cache
{
    class PipelineStateCache;
} // namespace Ame::Gfx::Cache

namespace Ame::Gfx::RG::Std
{
    class EntityCollectPass : public Pass
    {
    public:
        static constexpr int32_t  MinEntities     = 1'024;
        static constexpr uint32_t BufferAlignment = 0xFFFF;

        struct DispatchDesc
        {
            uint32_t IndexOffset;
            uint32_t IndexCount;
            uint32_t VertexOffset;
        };

        EntityCollectPass(
            Ecs::Universe&             Universe,
            Cache::PipelineStateCache& PipelineStateCache);

    private:
        Ref<Ecs::Universe>             m_Universe;
        Ref<Cache::PipelineStateCache> m_PipelineStateCache;
    };
} // namespace Ame::Gfx::RG::Std