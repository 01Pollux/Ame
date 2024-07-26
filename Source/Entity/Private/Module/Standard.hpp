#pragma once

#include <Ecs/Core.hpp>

namespace Ame::Ecs::Module
{
    class StandardModule
    {
    public:
        StandardModule(
            flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs::Module