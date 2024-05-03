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
    };
} // namespace Ame::Gfx::RG::Std