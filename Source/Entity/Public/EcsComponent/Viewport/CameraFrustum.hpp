#pragma once

#include <Math/Camera.hpp>
#include <Geometry/Frustum.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Component automatically set- and updated when the camera has a transform component.
    /// </summary>
    struct CameraFrustumComponent : Geometry::Frustum
    {
        using Geometry::Frustum::Frustum;

        CameraFrustumComponent() = default;
        CameraFrustumComponent(const Geometry::Frustum& frustum) :
            Geometry::Frustum(frustum)
        {
        }
    };
} // namespace Ame::Ecs