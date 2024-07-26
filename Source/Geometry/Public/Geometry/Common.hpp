#pragma once

#include <Math/Plane.hpp>
#include <Math/Transform.hpp>
#include <Math/Ray.hpp>
#include <DirectXCollision.h>

namespace Ame::Geometry
{
    using ContainmentType       = Math::Util::ContainmentType;
    using PlaneIntersectionType = Math::Util::PlaneIntersectionType;

    //

    struct AABB;
    struct Frustum;
    struct OBB;
    struct Sphere;
} // namespace Ame::Geometry