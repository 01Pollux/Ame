#pragma once

#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct OBB : Math::Util::BoundingOrientedBox
    {
    public:
        using BoundingOrientedBox::BoundingOrientedBox;

    public:
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