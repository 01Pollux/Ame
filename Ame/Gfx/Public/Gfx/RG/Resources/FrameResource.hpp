#pragma once

#include <Gfx/RG/Core.hpp>

#include <Math/Transform.hpp>
#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>

#include <Ecs/Entity.hpp>

namespace Ame::Gfx::RG
{
    struct FrameResource
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

        Ecs::Entity CurrentCamera;
    };
} // namespace Ame::Gfx::RG