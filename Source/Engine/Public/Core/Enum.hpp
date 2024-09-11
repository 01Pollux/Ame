#pragma once

#include <Core/Ame.hpp>
#include <magic_enum.hpp>
#include <magic_enum_containers.hpp>

namespace Ame
{
    namespace Enum             = magic_enum;
    namespace EnumBitOperators = Enum::bitwise_operators;

    template<typename Type> using EnumBitSet = Enum::containers::bitset<Type>;
} // namespace Ame