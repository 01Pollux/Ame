#pragma once

#include <RG/Core.hpp>

#include <Math/Transform.hpp>
#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>

#include <Ecs/Entity.hpp>

namespace Ame::RG
{
    // matrices are column-major
    struct FrameResourceGPU
    {
        Math::Matrix4x4 World;

        Math::Matrix4x4 View;
        Math::Matrix4x4 Projection;
        Math::Matrix4x4 ViewProjection;

        Math::Matrix4x4 ViewInverse;
        Math::Matrix4x4 ProjectionInverse;
        Math::Matrix4x4 ViewProjectionInverse;

        Math::Vector2 Viewport;

        float EngineTime;
        float GameTime;
        float DeltaTime;
    };

    struct FrameResourceCPU : FrameResourceGPU
    {
        /// <summary>
        /// Camera can be null indicating that the graph is being rendered without a camera.
        /// In that case, world and view matrices are identity matrices.
        /// </summary>
        Ecs::Entity CurrentCamera;
    };
} // namespace Ame::RG