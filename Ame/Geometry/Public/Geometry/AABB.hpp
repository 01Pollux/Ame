#pragma once

#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct AABB : Math::Util::BoundingBox
    {
    public:
        using BoundingBox::BoundingBox;

    public:
        [[nodiscard]] static AABB FromMinMax(
            const Math::Vector3& min,
            const Math::Vector3& max) noexcept
        {
            return AABB((min + max) * 0.5f, (max - min) * 0.5f);
        }

        [[nodiscard]] Math::Vector3 Min() const noexcept
        {
            return Math::Vector3(Center) - Extents;
        }

        [[nodiscard]] Math::Vector3 Max() const noexcept
        {
            return Math::Vector3(Center) + Extents;
        }

        [[nodiscard]] Math::Vector3 Size() const noexcept
        {
            return Math::Vector3(Extents) * 2.0f;
        }
    };
} // namespace Ame::Geometry