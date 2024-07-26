#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs::Component
{
    struct Transform;
    struct AABB;
} // namespace Ame::Ecs::Component

namespace Ame::Ecs::Module
{
    class MathModule
    {
    public:
        MathModule(
            flecs::world& flecsWorld);

    private:
        /// <summary>
        /// Register the observer for the transform and other components.
        /// </summary>
        void RegisterTransformObserver(
            flecs::world& flecsWorld);

        static void OnTransformChanged(
            Iterator&                   iter,
            const Component::Transform* transforms,
            const Component::AABB*      boxes);
    };
} // namespace Ame::Ecs::Module