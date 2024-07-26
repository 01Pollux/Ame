#pragma once

#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct Frustum : Math::Util::BoundingFrustum
    {
    public:
        using BoundingFrustum::BoundingFrustum;

    public:
        using BoundingFrustum::Transform;

        [[nodiscard]] Frustum Transform(
            const Math::Matrix4x4& transform) const noexcept
        {
            auto copy = *this;
            Transform(copy, transform);
            return copy;
        }
    };
} // namespace Ame::Geometry