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
            Buffer&     RhiBuffer,
            BufferRange Range) :
            RhiBuffer(RhiBuffer),
            Range(Range.Transform(RhiBuffer))
        {
        }
    };
} // namespace Ame::Rhi::Streaming
