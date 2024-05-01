#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs
{
    template<typename... ArgsTy>
    using FilterBuilder = flecs::filter_builder<ArgsTy...>;

    template<typename... ArgsTy>
    using Filter = flecs::filter<ArgsTy...>;

    template<typename... ArgsTy>
    using UniqueFilter = Unique<Filter<ArgsTy...>>;
} // namespace Ame::Ecs