#pragma once

#include <Geometry/Frustum.hpp>

namespace Ame::Geometry
{
    struct FrustumRadar : Frustum
    {
    public:
        /// <summary>
        /// Get planes for frustum
        /// </summary>
        [[nodiscard]] std::array<Math::Plane, 6> GetPlanes() const;

    public:
        /// <summary>
        /// Transform the frustum
        /// </summary>
        void Transform(
            const Math::TransformMatrix& transform)
        {
            Transform(transform.GetScale(), transform.GetRotation(), transform.GetPosition());
        }

        /// <summary>
        /// Transform the frustum
        /// </summary>
        void Transform(
            const Math::Vector3&    scale,
            const Math::Quaternion& rotation,
            const Math::Vector3&    translation);

    public:
        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const AABB& box) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const Frustum& fr) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            std::span<const Math::Plane> planes) const;
    };
} // namespace Ame::Geometry