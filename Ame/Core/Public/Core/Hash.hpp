#pragma once

#include <Core/Ame.hpp>

namespace Ame
{
    template<class T>
    inline void HashCombine(
        std::size_t& seed,
        const T&     value)
    {
        constexpr uint64_t c_GoldenRatio = 0x9e3779b9;
        seed ^= std::hash<T>{}(value) + c_GoldenRatio + (seed << 6) + (seed >> 2);
    }
} // namespace Ame
