#pragma once

#include <Math/Vector.hpp>

namespace Ame::Math
{
    template<typename Ty> struct Size2T : Ty
    {
    public:
        using Ty::Ty;

        using value_type = typename Ty::value_type;

        [[nodiscard]] value_type Width() const
        {
            return this->x();
        }

        void Width(value_type val)
        {
            this->x() = val;
        }

        [[nodiscard]] value_type Height() const
        {
            return this->y();
        }

        void Height(value_type val)
        {
            this->y() = val;
        }
    };

    using Size2  = Size2T<Vector2>;
    using Size2I = Size2T<Vector2I>;
    using Size2U = Size2T<Vector2U>;

    template<typename Ty>
    concept Size2Type = std::is_same_v<Ty, Size2> || std::is_same_v<Ty, Size2I> || std::is_same_v<Ty, Size2U>;
} // namespace Ame::Math
