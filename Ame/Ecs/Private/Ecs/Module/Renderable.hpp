#pragma once

#include <Ecs/Core.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Ecs::Module
{
    class RenderableModule
    {
    public:
        RenderableModule(
            flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs::Module