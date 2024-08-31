#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs
{
    template<typename... ArgsTy> using QueryBuilder = flecs::query_builder<ArgsTy...>;

    template<typename... ArgsTy> using Query = flecs::query<ArgsTy...>;

    template<typename... ArgsTy> using UniqueQuery = Unique<Query<ArgsTy...>>;
} // namespace Ame::Ecs