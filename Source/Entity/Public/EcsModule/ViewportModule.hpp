#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs
{
    class ViewporEcstModule
    {
    public:
        ViewporEcstModule(
            flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs