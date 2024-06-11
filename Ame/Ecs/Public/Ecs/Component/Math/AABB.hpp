#pragma once

#include <Geometry/AABB.hpp>

namespace Ame::Ecs::Component
{
    struct AABB : public Geometry::AABBMinMax
    {
    };
} // namespace Ame::Ecs::Component