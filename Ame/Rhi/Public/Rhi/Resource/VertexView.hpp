#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Rhi
{
    struct VertexBufferView
    {
        Rhi::Buffer Buffer;
        uint64_t    Offset = 0;
    };

    struct IndexBufferView
    {
        Rhi::Buffer    Buffer;
        uint64_t       Offset = 0;
        Rhi::IndexType Type   = Rhi::IndexType::UINT16;
    };
} // namespace Ame::Rhi