#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// TransformComponent
    /// GlobalTransformComponent
    /// AABBComponent
    /// TransformedAABBComponent
    /// </summary>
    class AME_ENGINE_API MathEcsModule
    {
    public:
        MathEcsModule(WorldRef world);

    private:
        void RegisterTransformObservers(WorldRef world);
    };
} // namespace Ame::Ecs