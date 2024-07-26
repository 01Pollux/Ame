#pragma once

#include <Geometry/AABB.hpp>

namespace Ame::Ecs
{
    struct AABBComponent : public Geometry::AABBMinMax
    {
    };

    /// <summary>
    /// Component set automatically when an entity has a transform and an AABB.
    /// </summary>
    struct TransformedAABBComponent : public Geometry::AABBMinMax
    {
    };
} // namespace Ame::Ecs::Component