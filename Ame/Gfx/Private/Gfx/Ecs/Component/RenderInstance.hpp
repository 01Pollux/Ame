#pragma once

#include <Math/Vector.hpp>

#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Ecs::Gfx::Component
{
    struct RenderInstance
    {
        uint32_t AABBIndex;
        uint32_t TransformIndex;
        uint32_t CameraMask;

        uint32_t VertexOffset;
        uint32_t VertexSize;
        uint32_t IndexOffset;
        uint32_t IndexCount;
    };
} // namespace Ame::Ecs::Gfx::Component