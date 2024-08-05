#pragma once

#include <Core/Interface.hpp>
#include <Math/Camera.hpp>
#include <Geometry/Frustum.hpp>
#include <RG/Graph.hpp>

namespace Ame::Ecs
{
    struct CameraComponent : Math::Camera
    {
        Ptr<RG::Graph> RenderGraph;
    };

    /// <summary>
    /// Component automatically set- and updated when the camera has a transform component.
    /// </summary>
    struct CameraFrustum
    {
        Geometry::Frustum Frustum;
    };
} // namespace Ame::Ecs