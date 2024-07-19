#pragma once

#include <Core/Ame.hpp>

namespace Ame
{
    template<class Ty>
    inline void HashCombine(
        std::size_t& seed,
        const Ty&    value)
    {
        constexpr uint64_t c_GoldenRatio = 0x9e3779b9;
        seed ^= std::hash<Ty>{}(value) + c_GoldenRatio + (seed << 6) + (seed >> 2);
    }

    template<typename Ty>
    inline void HashCombine(
        std::size_t& seed,
        const Ty*    first,
        const Ty*    last)
    {
        for (; first != last; ++first)
        {
            HashCombine(seed, *first);
        }
    }
} // namespace Ame
