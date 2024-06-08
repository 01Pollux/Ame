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
        Math::Vector3 Center = Math::Vec::c_Zero<Math::Vector3>;

        /// <summary>
        /// Distance from the center to each side.
        /// </summary>
        Math::Vector3 Extents{ std::numeric_limits<float>::max() };

    public:
        /// <summary>
        /// Construct a new AABB from a center and extents.
        /// </summary>
        void FromMinMax(
            const Math::Vector3& min,
            const Math::Vector3& max)
        {
            Center  = (min + max) / 2.0f;
            Extents = (max - min) / 2.0f;
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
            const Math::TransformMatrix& tr)
        {
            Transform(tr.GetScale(), tr.GetRotation(), tr.GetPosition());
        }

        /// <summary>
        /// Transform the aabb
        /// </summary>
        void Transform(
            const Math::Vector3&    scale,
            const Math::Quaternion& rotation,
            const Math::Vector3&    translation);
    };
} // namespace Ame::Geometry