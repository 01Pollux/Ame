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
            uint32_t DrawOffset;
            uint32_t DrawCount;
            uint32_t CounterOffset;
        };

        EntityCollectPass(
            Ecs::Universe&             Universe,
            Cache::PipelineStateCache& PipelineStateCache);

    private:
        Ref<Ecs::Universe>             m_Universe;
        Ref<Cache::PipelineStateCache> m_PipelineStateCache;
        uint32_t                       m_MaxEntitiesCount;
    };
} // namespace Ame::Gfx::RG::Std