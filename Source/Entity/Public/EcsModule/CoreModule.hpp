#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Imports:
    /// MathEcsModule
    /// 
    /// Defines:
    /// EntityTagComponent
    /// </summary>
    class CoreEcsModule
    {
    public:
        CoreEcsModule(
            flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs