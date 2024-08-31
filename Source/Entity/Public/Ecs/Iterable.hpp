#pragma once

#include <Ecs/Core.hpp>

namespace Ame::Ecs
{
    template<typename... ArgsTy> using Iterable = flecs::iter_iterable<ArgsTy...>;

    using Iterator = flecs::iter;
} // namespace Ame::Ecs