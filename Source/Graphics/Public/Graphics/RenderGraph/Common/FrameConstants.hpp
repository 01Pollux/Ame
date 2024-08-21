#pragma once

#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>

namespace Ame::Gfx
{
    struct CameraFrameDataUpdateDesc
    {
        Math::Matrix4x4 World;
        Math::Matrix4x4 View;
        Math::Matrix4x4 Projection;
        Math::Vector2   Viewport;

        float EngineTime;
        float GameTime;
        float DeltaTime;

        float NearPlane;
        float FarPlane;

        uint32_t CameraMask;
    };

    struct CameraFrameData
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

        float NearPlane;
        float FarPlane;

        uint32_t CameraMask;
    };
    static_assert(sizeof(CameraFrameData) == Math::AlignUp(sizeof(CameraFrameData), 16), "CameraFrameData is not 16-byte aligned");
} // namespace Ame::Gfx