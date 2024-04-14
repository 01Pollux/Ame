#pragma once

#include <Rhi/Core.hpp>
#include <Rhi/Resource/View.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Rhi::Streaming
{
    struct BufferView
    {
        Buffer&     RhiBuffer;
        BufferRange Range;

        BufferView(
            Device&     RhiDevice,
            Buffer&     RhiBuffer,
            BufferRange Range) :
            RhiBuffer(RhiBuffer),
            Range(Range.Transform(RhiDevice, RhiBuffer))
        {
        }
    };
} // namespace Ame::Rhi
