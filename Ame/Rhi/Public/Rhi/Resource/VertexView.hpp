#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Rhi
{
    struct VertexBufferView
    {
        nri::Buffer* NriBuffer = nullptr;
        uint64_t     Offset = 0;
    };

    struct IndexBufferView
    {
        nri::Buffer*   NriBuffer = nullptr;
        uint64_t       Offset = 0;
        Rhi::IndexType Type   = Rhi::IndexType::UINT16;
    };
} // namespace Ame::Rhi