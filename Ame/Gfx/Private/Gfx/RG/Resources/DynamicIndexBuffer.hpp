#pragma once

#include <Rhi/Util/BlockBasedBuffer.hpp>

namespace Ame::Gfx::RG
{
    class DynamicIndexBuffer : public Rhi::Util::BlockBasedBuffer<>
    {
    public:
        using BlockBasedBuffer::BlockBasedBuffer;
    };
} // namespace Ame::Gfx::RG