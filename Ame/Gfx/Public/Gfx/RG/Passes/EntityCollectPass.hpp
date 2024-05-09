#pragma once

#include <Gfx/RG/Pass.hpp>
#include <Gfx/Cache/StdPipelineLayouts.hpp>
#include <Ecs/Universe.hpp>

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
            Ecs::Universe&              Universe,
            Cache::PipelineLayoutCache& LayoutCache);

    private:
        Ref<Ecs::Universe>              m_Universe;
        Ref<Cache::PipelineLayoutCache> m_LayoutCache;
    };
} // namespace Ame::Gfx::RG::Std