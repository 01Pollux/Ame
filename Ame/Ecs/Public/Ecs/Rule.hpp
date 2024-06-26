#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs
{
    template<typename... ArgsTy>
    using RuleBuilder = flecs::rule_builder<ArgsTy...>;

    template<typename... ArgsTy>
    using Rule = flecs::rule<ArgsTy...>;

    template<typename... ArgsTy>
    using UniqueRule = Unique<Rule<ArgsTy...>>;
} // namespace Ame::Ecs