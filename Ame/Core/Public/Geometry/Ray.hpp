#pragma once

#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct Ray
    {
        Math::Vector3 Origin;
        Math::Vector3 Direction;

        Ray() = default;
        Ray(
            const Math::Vector3& Origin,
            const Math::Vector3& Direction) :
            Origin(Origin),
            Direction(Direction)
        {
        }

    public:
        /// <summary>
        /// Returns a point along the ray at a distance t from the origin.
        /// </summary>
        [[nodiscard]] Math::Vector3 PointAt(
            float t) const
        {
            return Origin + t * Direction;
        }

    public:
        /// <summary>
        /// Returns a ray that has been transformed by the given matrix.
        /// </summary>
        [[nodiscard]] Ray Transform(
            const Math::Matrix4x4& Mat) const
        {
            return Ray(
                Math::Vector3(Mat * Math::Vector4(Origin, 1.0f)),
                Math::Vector3(Mat * Math::Vector4(Direction, 0.0f)));
        }

        /// <summary>
        /// Check intersection between two lines.
        /// Disjoint are parallel
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const Ray& Other,
            float*     T = nullptr,
            float*     S = nullptr)
        {
            Math::Vector3 V1V2 = glm::cross(Other.Origin, Origin);
            float         Dot2 = glm::pow(glm::dot(V1V2, V1V2), 2.f);

            if (!Dot2)
            {
                return ContainmentType::Disjoint;
            }

            if (T || S)
            {
                Math::Matrix3x3 M(Other.Origin - Origin, Other.Direction, V1V2);
                Math::Vector3   R = glm::inverse(M) * Math::Vector3(0.f, 0.f, 1.f);

                if (T)
                {
                    *T = R.x;
                }

                if (S)
                {
                    *S = R.y;
                }
            }

            return ContainmentType::Intersects;
        }
    };
} // namespace Ame::Geometry