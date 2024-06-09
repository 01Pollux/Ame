#pragma once

#include <Math/Plane.hpp>
#include <Math/Transform.hpp>
#include <DirectXCollision.h>

namespace Ame::Geometry
{
    static constexpr Math::Vector3 c_BoxOffset[8]{
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f }
    };

    //

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
} // namespace Ame::Geometry