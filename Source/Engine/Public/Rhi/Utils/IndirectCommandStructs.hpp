#pragma once

#include <cstdint>

namespace Ame::Rhi
{
    struct DrawIndirectCommand
    {
        uint32_t NumVertices;
        uint32_t NumInstances;
        uint32_t StartVertexLocation;
        uint32_t FirstInstanceLocation;
    };

    struct DrawIndexedIndirectCommand
    {
        uint32_t NumIndices;
        uint32_t NumInstances;
        uint32_t FirstIndexLocation;
        uint32_t BaseVertex;
        uint32_t FirstInstanceLocation;
    };
} // namespace Ame::Rhi