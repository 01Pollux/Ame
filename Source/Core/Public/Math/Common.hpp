#pragma once

#include <concepts>
#include <cstdlib>

namespace Ame::Math
{
    template<std::integral Ty>
    [[nodiscard]] constexpr Ty AlignUpWithMask(
        Ty     value,
        size_t mask) noexcept
    {
        return Ty((size_t(value) + mask) & ~mask);
    }

    template<std::integral Ty>
    [[nodiscard]] constexpr Ty AlignDownWithMask(
        Ty     value,
        size_t mask) noexcept
    {
        return Ty(size_t(value) & ~mask);
    }

    template<std::integral Ty>
    [[nodiscard]] constexpr Ty AlignUp(
        Ty     value,
        size_t alignment) noexcept
    {
        return AlignUpWithMask(value, alignment - 1);
    }

    template<std::integral Ty>
    [[nodiscard]] constexpr Ty AlignDown(
        Ty     value,
        size_t alignment) noexcept
    {
        return AlignDownWithMask(value, alignment - 1);
    }

    template<std::integral Ty>
    [[nodiscard]] constexpr bool IsAligned(
        Ty     value,
        size_t alignment) noexcept
    {
        return !(size_t(value) & (alignment - 1));
    }

    template<std::integral Ty>
    [[nodiscard]] constexpr Ty DivideByMultiple(
        Ty     value,
        size_t alignment) noexcept
    {
        return Ty((value + alignment - 1) / alignment);
    }

    template<std::integral Ty>
    [[nodiscard]] constexpr bool IsPowerOfTwo(
        Ty value) noexcept
    {
        return !(value & (value - 1));
    }

    template<std::integral Ty>
    [[nodiscard]] constexpr bool IsDivisible(
        Ty value,
        Ty divisor) noexcept
    {
        return ((value / divisor) * divisor) == value;
    }

    /// Returns the least-significant bit and clears it in the input argument
    template<std::integral Ty>
    [[nodiscard]] constexpr Ty ExtractLSB(
        Ty& bits)
    {
        if (bits == Ty{ 0 })
        {
            return 0;
        }

        const Ty bit = bits & ~(bits - Ty{ 1 });
        bits &= ~bit;

        return bit;
    }

    /// Returns the enum value representing the least-significant bit and clears it in the input argument
    template<typename Ty>
        requires std::is_enum_v<Ty>
    [[nodiscard]] constexpr Ty ExtractLSB(
        Ty& bits)
    {
        using UnderlyingTy = std::underlying_type_t<Ty>;

        UnderlyingTy       underlyingBits = static_cast<UnderlyingTy>(bits);
        const UnderlyingTy bit            = ExtractLSB(underlyingBits);
        bits                              = static_cast<Ty>(underlyingBits);

        return static_cast<Ty>(bit);
    }
} // namespace Ame::Math
