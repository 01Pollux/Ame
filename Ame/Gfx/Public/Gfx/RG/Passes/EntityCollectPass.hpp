#pragma once

#include <Gfx/RG/Pass.hpp>
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
            Ecs::Universe& Universe);

    private:
        Ref<Ecs::Universe> m_Universe;
    };
} // namespace Ame::Gfx::RG::Std