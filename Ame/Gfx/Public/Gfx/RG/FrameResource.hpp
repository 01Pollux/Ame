#pragma once

#include <Gfx/RG/Core.hpp>
#include <Math/Transform.hpp>
#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>

namespace Ame::Gfx::RG
{
    struct FrameResource
    {
        alignas(16) Math::Matrix4x4 World;

        alignas(16) Math::Matrix4x4 View;
        alignas(16) Math::Matrix4x4 Projection;
        alignas(16) Math::Matrix4x4 ViewProjection;

        alignas(16) Math::Matrix4x4 ViewInverse;
        alignas(16) Math::Matrix4x4 ProjectionInverse;
        alignas(16) Math::Matrix4x4 ViewProjectionInverse;

        Math::Vector2 Viewport;

        float EngineTime;
        float GameTime;
        float DeltaTime;
    };
} // namespace Ame::Gfx::RG