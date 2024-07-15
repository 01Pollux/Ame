#pragma once

#include <Math/Vector.hpp>

namespace Ame::Math
{
    template<typename VecTy>
    class Ray
    {
    public:
        using vector_type = VecTy;

    public:
        vector_type Origin;
        vector_type Direction;

    public:
        constexpr Ray(
            const vector_type& origin,
            const vector_type& direction) noexcept :
            Origin(origin),
            Direction(direction)
        {
        }

    public:
        Ray& operator*=(const Matrix4x4& matrix) noexcept
        {
            Origin    = matrix * Origin;
            Direction = matrix * Direction;
            return *this;
        }
        [[nodiscard]] Ray operator*(const Matrix4x4& matrix) const noexcept
        {
            return Ray(matrix * Origin, matrix * Direction);
        }

        Ray& operator*=(const Matrix3x3& matrix) noexcept
        {
            Direction = matrix * Direction;
            return *this;
        }
        [[nodiscard]] Ray operator*(const Matrix3x3& matrix) const noexcept
        {
            return Ray(Origin, matrix * Direction);
        }

        Ray& operator*=(const Quaternion& quaternion) noexcept
        {
            Direction = quaternion * Direction;
            return *this;
        }
        [[nodiscard]] Ray operator*(const Quaternion& quaternion) const noexcept
        {
            return Ray(Origin, quaternion * Direction);
        }
    };
} // namespace Ame::Math
