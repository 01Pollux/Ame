#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// TransformComponent
    /// GlobalTransformComponent
    /// AABBComponent
    /// AABBTransformedComponent
    /// </summary>
    class MathEcsModule
    {
    public:
        MathEcsModule(
            flecs::world& flecsWorld);

    private:
        void RegisterTransformObservers(
            flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs