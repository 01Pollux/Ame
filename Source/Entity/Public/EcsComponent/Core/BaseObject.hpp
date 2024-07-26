#pragma once

#include <flecs.h>

namespace Ame::Ecs
{
    template<typename Ty, typename BaseTy>
    static flecs::component<Ty> ImplementObject(
        flecs::world& world,
        const char*   name)
    {
        auto component = world.component<Ty>(name);
        component
            .on_set([](flecs::entity e, Ty& c)
                    { e.emplace<BaseTy>(c.Object); })
            .on_remove([](flecs::entity e, Ty&)
                       { e.remove<BaseTy>(); })
            .is_a<BaseTy>();
        return component;
    }
} // namespace Ame::Ecs