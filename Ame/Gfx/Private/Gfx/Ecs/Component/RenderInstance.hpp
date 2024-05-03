#pragma once

#include <Math/Vector.hpp>

namespace Ame::Ecs::Gfx::Component
{
    struct RenderInstance
    {
        uint32_t AABBIndex;
        uint32_t TransformIndex;
        uint32_t CameraMask;

        // if VertexOffset is -1, then the instance will be using the mesh's vertex buffer
        uint32_t VertexOffset;
        uint32_t VertexSize;
        uint32_t IndexOffset;
        uint32_t IndexCount;
    };
} // namespace Ame::Ecs::Gfx::Component