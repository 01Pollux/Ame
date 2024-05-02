#pragma once

#include <Ecs/Core.hpp>

#include <Ecs/Component/Renderable/Renderable.hpp>

namespace Ame::Ecs::Module
{
    class RenderableModule
    {
    public:
        RenderableModule(
            flecs::world& FlecsWorld);

    public:
        /// <summary>
        /// Attaches the Renderable component to the given FlecsComponent.
        /// Used to mark a component as renderable.
        /// </summary>
        template<typename Ty>
        static flecs::component<Ty> AttachRenderable(
            flecs::component<Ty> FlecsComponent)
        {
            return FlecsComponent
                .on_add(
                    [](flecs::entity FlecsEntity, Ty&)
                    { FlecsEntity.add<Component::Renderable>(); })
                .on_remove(
                    [](flecs::entity FlecsEntity, Ty&)
                    { FlecsEntity.remove<Component::Renderable>(); });
        }
    };
} // namespace Ame::Ecs::Module