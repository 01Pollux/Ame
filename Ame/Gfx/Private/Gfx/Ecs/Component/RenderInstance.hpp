#pragma once

#include <Math/Vector.hpp>

#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Ecs::Gfx::Component
{
    struct RenderInstance
    {
        uint32_t AABBIndex;
        uint32_t TransformIndex;
        uint32_t MaterialIndex;
        uint32_t VisibilityIndex;

        uint32_t VertexOffset;
        uint32_t IndexOffset;
        uint32_t IndexCount;

        Rhi::Buffer VertexBuffer;
        Rhi::Buffer IndexBuffer;
    };
} // namespace Ame::Ecs::Gfx::Component