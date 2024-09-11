#pragma once

#include <Math/Vector.hpp>

namespace Ame::Math
{
    template<typename Ty> struct RectT
    {
        using value_type = typename Ty::value_type;

        Ty Position;
        Ty Size;

        constexpr explicit RectT(const Ty& position = {}, const Ty& size = {}) noexcept : Position(position), Size(size)
        {
        }

    public:
        [[nodiscard]] constexpr value_type Width() const noexcept
        {
            return Size.x();
        }

        [[nodiscard]] constexpr value_type Height() const noexcept
        {
            return Size.y();
        }

    public:
        [[nodiscard]] constexpr value_type Left() const noexcept
        {
            return Position.x();
        }

        [[nodiscard]] constexpr value_type Top() const noexcept
        {
            return Position.y();
        }

        [[nodiscard]] constexpr value_type Right() const noexcept
        {
            return Position.x() + Width();
        }

        [[nodiscard]] constexpr value_type Bottom() const noexcept
        {
            return Position.y() + Height();
        }

    public:
        [[nodiscard]] constexpr Ty TopLeft() const noexcept
        {
            return Ty(Left(), Top());
        }

        [[nodiscard]] constexpr Ty TopRight() const noexcept
        {
            return Ty(Right(), Top());
        }

        [[nodiscard]] constexpr Ty BottomLeft() const noexcept
        {
            return Ty(Left(), Bottom());
        }

        [[nodiscard]] constexpr Ty BottomRight() const noexcept
        {
            return Ty(Right(), Bottom());
        }
    };

    using RectI = RectT<Vector2I>;
    using RectF = RectT<Vector2>;

    template<typename Ty>
    concept RectType = std::is_same_v<Ty, RectI> || std::is_same_v<Ty, RectF>;
} // namespace Ame::Math
