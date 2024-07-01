#pragma once

#include <Math/Camera.hpp>
#include <Geometry/Frustum.hpp>

namespace Ame::Math
{
    class TransformMatrix;
} // namespace Ame::Math

namespace Ame::Ecs::Component
{
    struct Camera : Math::Camera
    {
    };

    /// <summary>
    /// Component automatically set- and updated when the camera has a transform component.
    /// </summary>
    struct CameraFrustum
    {
        Geometry::Frustum Frustum;
    };
} // namespace Ame::Ecs::Component