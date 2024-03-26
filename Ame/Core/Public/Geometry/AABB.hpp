#pragma once

#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct AABB
    {
    public:
        /// <summary>
        /// Center of the box.
        /// </summary>
        Math::Vector3 Center = Math::Vec::Zero<Math::Vector3>;

        /// <summary>
        /// Distance from the center to each side.
        /// </summary>
        Math::Vector3 Extents{ std::numeric_limits<float>::max() };

    public:
        /// <summary>
        /// Construct a new AABB from a center and extents.
        /// </summary>
        void FromMinMax(
            const Math::Vector3& Min,
            const Math::Vector3& Max)
        {
            Center  = (Min + Max) / 2.0f;
            Extents = (Max - Min) / 2.0f;
        }

    public:
        [[nodiscard]] Math::Vector3 Min() const
        {
            return Center - Extents;
        }
        [[nodiscard]] Math::Vector3 Max() const
        {
            return Center + Extents;
        }

    public:
        /// <summary>
        /// Transform the aabb
        /// </summary>
        void Transform(
            const Math::TransformMatrix& Tr)
        {
            Transform(Tr.GetScale(), Tr.GetRotation(), Tr.GetPosition());
        }

        /// <summary>
        /// Transform the aabb
        /// </summary>
        void Transform(
            const Math::Vector3&    Scale,
            const Math::Quaternion& Rotation,
            const Math::Vector3&    Translation);

    public:
        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const AABB& Box) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            const Frustum& Fr) const;

        /// <summary>
        /// Check collision
        /// </summary>
        [[nodiscard]] ContainmentType Contains(
            std::span<const Math::Plane> Planes) const;
    };
} // namespace Ame::Geometry