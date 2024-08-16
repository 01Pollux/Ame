#pragma once

#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>

namespace Ame::Gfx
{
    struct CameraFrameDataUpdateDesc
    {
        Math::Matrix4x4 WorldTransposed;
        Math::Matrix4x4 ViewTransposed;
        Math::Matrix4x4 ProjectionTransposed;
        Math::Vector2   Viewport;

        float EngineTime;
        float GameTime;
        float DeltaTime;

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

        uint32_t CameraMask;
        uint32_t _padding[2];
    };
    static_assert(sizeof(CameraFrameData) == Math::AlignUp(sizeof(CameraFrameData), 16), "CameraFrameData is not 16-byte aligned");
} // namespace Ame::Gfx