#pragma once

#include <Gfx/RG/Pass.hpp>
#include <Ecs/Universe.hpp>

#include <Gfx/Draw/Batcher/Batcher.hpp>

namespace Ame::Gfx::RG::Std
{
    class GeometryPass : public Pass
    {
    public:
        GeometryPass(
            Ecs::Universe& Universe);

    private:
        Ref<Ecs::Universe> m_Universe;

        // TODO: Maybe this should be related to the scene storage, where we have access to cameras
        DrawBatcher m_DrawBatcher;
    };
} // namespace Ame::Gfx::RG::Std