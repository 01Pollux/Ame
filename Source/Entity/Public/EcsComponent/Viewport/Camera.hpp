#pragma once

#include <Core/Interface.hpp>
#include <Math/Camera.hpp>
#include <Geometry/Frustum.hpp>
#include <RG/Graph.hpp>

namespace Ame::Ecs
{
    struct CameraComponent : Math::Camera
    {
        /// <summary>
        /// The culling mask of the camera.
        /// </summary>
        uint32_t ViewMask = 0xFFFFFFFF;

        /// <summary>
        /// The render order of the camera.
        /// </summary>
        int Priority = 0;

        Ptr<RG::Graph> RenderGraph;
    };
} // namespace Ame::Ecs