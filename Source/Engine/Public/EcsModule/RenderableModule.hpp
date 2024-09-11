#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// IBaseRenderableComponent
    /// </summary>
    class AME_ENGINE_API RenderableEcsModule
    {
    public:
        RenderableEcsModule(flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs