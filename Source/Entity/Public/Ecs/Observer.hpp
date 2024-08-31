#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs
{
    template<typename... ArgsTy> using ObserverBuilder = flecs::observer_builder<ArgsTy...>;

    using Observer = flecs::observer;

    using UniqueObserver = Unique<Observer>;
} // namespace Ame::Ecs