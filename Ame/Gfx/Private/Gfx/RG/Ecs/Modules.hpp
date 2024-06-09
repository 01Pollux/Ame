#pragma once

#include <Gfx/RG/Ecs/System.hpp>

namespace Ame::Gfx::RG
{
    class RGModule
    {
    public:
        RGModule(
            flecs::world& flecsWorld);
    };
} // namespace Ame::Gfx::RG