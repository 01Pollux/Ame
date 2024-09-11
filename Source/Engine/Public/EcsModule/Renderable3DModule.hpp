#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// IBaseRenderable3DComponent
    /// StaticMeshComponent
    /// </summary>
    class Renderable3DEcsModule
    {
    public:
        Renderable3DEcsModule(flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs