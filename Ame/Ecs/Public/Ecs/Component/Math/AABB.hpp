#pragma once

#include <Geometry/AABB.hpp>

namespace Ame::Ecs::Component
{
    struct AABB : public Geometry::AABBMinMax
    {
    };

    /// <summary>
    /// Component set automatically when an entity has a transform and an AABB.
    /// </summary>
    struct AABBTransformed
    {
    public:
        Geometry::AABB BoxTransformed;
    };
} // namespace Ame::Ecs::Component