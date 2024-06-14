#pragma once

#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct Frustum : Math::Util::BoundingFrustum
    {
        using BoundingFrustum::BoundingFrustum;
    };
} // namespace Ame::Geometry