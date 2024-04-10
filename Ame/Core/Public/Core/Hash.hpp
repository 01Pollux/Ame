#pragma once

#include <Core/Ame.hpp>

namespace Ame
{
    template<class T>
    inline void HashCombine(
        std::size_t& Seed,
        const T&     V)
    {
        constexpr uint64_t GoldenRatio = 0x9e3779b9;
        Seed ^= std::hash<T>{}(V) + GoldenRatio + (Seed << 6) + (Seed >> 2);
    }
} // namespace Ame
