#pragma once

#include <limits>

namespace Ame::Gfx::RG
{
    struct RenderInstance
    {
        uint32_t AABBId      = std::numeric_limits<uint32_t>::max();
        uint32_t TransformId = std::numeric_limits<uint32_t>::max();

        uint32_t VertexOffset = std::numeric_limits<uint32_t>::max();
        uint32_t VertexSize   = std::numeric_limits<uint32_t>::max();

        uint32_t IndexOffset = std::numeric_limits<uint32_t>::max();
        uint32_t IndexCount  = std::numeric_limits<uint32_t>::max();

        uint32_t Padding[2];
    };
} // namespace Ame::Gfx::RG