#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs::Component
{
    struct Transform;
    struct Camera;
} // namespace Ame::Ecs::Component

namespace Ame::Ecs::Module
{
    class ViewportModule
    {
    public:
        ViewportModule(
            flecs::world& flecsWorld);

    private:
        /// <summary>
        /// Register the observer for the transform component.
        /// </summary>
        void RegisterTransformObserver(
            flecs::world& flecsWorld);

        static void OnTransformChanged(
            Iterator&                   iter,
            const Component::Transform* transforms);
    };
} // namespace Ame::Ecs::Module