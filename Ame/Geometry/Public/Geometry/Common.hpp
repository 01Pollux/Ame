#pragma once

#include <Math/Plane.hpp>
#include <Math/Transform.hpp>
#include <Math/Ray.hpp>
#include <DirectXCollision.h>

namespace Ame::Geometry
{
    using ContainmentType = Math::Util::ContainmentType;

    enum class PlaneIntersectionType : uint8_t
    {
        Front,
        Intersecting,
        Back
    };

    //

    struct AABB;
    struct Frustum;
    struct OBB;
    struct Sphere;
} // namespace Ame::Geometry