#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// RenderableComponent
    /// </summary>
    class RenderableEcsModule
    {
    public:
        RenderableEcsModule(flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs