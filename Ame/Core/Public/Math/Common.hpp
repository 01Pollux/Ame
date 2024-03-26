#pragma once

#include <concepts>
#include <cstdlib>

namespace Ame::Math
{
	template<std::integral Ty>
	[[nodiscard]] constexpr Ty AlignUpWithMask(
			Ty Value,
			size_t Mask) noexcept
	{
		return Ty((size_t(Value) + Mask) & ~Mask);
	}

	template<std::integral Ty>
	[[nodiscard]] constexpr Ty AlignDownWithMask(
			Ty Value,
			size_t Mask) noexcept
	{
		return Ty(size_t(Value) & ~Mask);
	}

	template<std::integral Ty>
	[[nodiscard]] constexpr Ty AlignUp(
			Ty Value,
			size_t Alignment) noexcept
	{
		return AlignUpWithMask(Value, Alignment - 1);
	}

	template<std::integral Ty>
	[[nodiscard]] constexpr Ty AlignDown(
			Ty Value,
			size_t Alignment) noexcept
	{
		return AlignDownWithMask(Value, Alignment - 1);
	}

	template<std::integral Ty>
	[[nodiscard]] constexpr bool IsAligned(
			Ty Value,
			size_t Alignment) noexcept
	{
		return !(size_t(Value) & (Alignment - 1));
	}

	template<std::integral Ty>
	[[nodiscard]] constexpr Ty DivideByMultiple(
			Ty Value,
			size_t Alignment) noexcept
	{
		return Ty((Value + Alignment - 1) / Alignment);
	}

	template<std::integral Ty>
	[[nodiscard]] constexpr bool IsPowerOfTwo(
			Ty Value) noexcept
	{
		return !(Value & (Value - 1));
	}

	template<std::integral Ty>
	[[nodiscard]] constexpr bool IsDivisible(
			Ty Value,
			Ty Divisor) noexcept
	{
		return ((Value / Divisor) * Divisor) == Value;
	}
} // namespace Ame::Math
