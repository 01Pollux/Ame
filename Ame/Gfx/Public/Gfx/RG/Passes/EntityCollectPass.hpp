#pragma once

#include <Gfx/RG/Pass.hpp>
#include <Ecs/Universe.hpp>

namespace Ame::Gfx::Cache
{
    class CommonPipelineState;
} // namespace Ame::Gfx::Cache

namespace Ame::Gfx::RG::Std
{
    class EntityCollectPass : public Pass
    {
    public:
        static constexpr uint32_t c_MinEntities     = 1'024;
        static constexpr uint32_t c_BufferAlignment = 0xFFFF;

        static constexpr uint32_t c_CommandInfo_SetIndex = 2;

        struct DispatchDesc
        {
            uint32_t DrawOffset;
            uint32_t DrawCount;
            uint32_t CounterOffset;
            uint32_t Pad;
        };

        EntityCollectPass(
            Ecs::Universe&              universe,
            Cache::CommonPipelineState& commonPipelines);

    private:
        Ref<Ecs::Universe>              m_Universe;
        Ref<Cache::CommonPipelineState> m_CommonPipelines;
        uint32_t                        m_MaxEntitiesCount;
    };
} // namespace Ame::Gfx::RG::Std