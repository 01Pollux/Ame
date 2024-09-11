#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs
{
    template<typename... ArgsTy> using SystemBuilder = flecs::system_builder<ArgsTy...>;

    using System = flecs::system;

    using UniqueSystem = Unique<System>;
} // namespace Ame::Ecs