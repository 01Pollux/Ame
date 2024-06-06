#pragma once

#include <Math/Vector.hpp>

namespace Ame::Gfx::RG
{
    struct RenderInstance
    {
        uint32_t AABBId      = std::numeric_limits<uint32_t>::max();
        uint32_t TransformId = std::numeric_limits<uint32_t>::max();

        uint32_t VertexOffset = std::numeric_limits<uint32_t>::max();
        uint32_t VertexSize   = std::numeric_limits<uint32_t>::max();
        uint32_t IndexOffset  = std::numeric_limits<uint32_t>::max();
        uint32_t IndexCount   = std::numeric_limits<uint32_t>::max();
    };
} // namespace Ame::Gfx::RG