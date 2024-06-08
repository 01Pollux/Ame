#pragma once

#include <array>
#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct FrustumPlanes
    {
        Math::Plane Near;
        Math::Plane Far;
        Math::Plane Right;
        Math::Plane Left;
        Math::Plane Top;
        Math::Plane Bottom;
    };
} // namespace Ame::Geometry