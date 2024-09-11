#pragma once

#include <flecs.h>

namespace Ame::Ecs
{
    template<typename Ty, typename BaseTy> inline flecs::component<Ty> ImplementObject(flecs::world& world)
    {
        auto component = world.component<Ty>();
        component.on_set([](flecs::entity e, Ty& c) { e.emplace<BaseTy>(c.Object); })
            .on_remove([](flecs::entity e, Ty&) { e.remove<BaseTy>(); });
        return component;
    }
} // namespace Ame::Ecs