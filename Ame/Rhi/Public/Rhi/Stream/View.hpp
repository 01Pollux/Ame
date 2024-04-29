#pragma once

#include <Rhi/Core.hpp>
#include <Rhi/Resource/View.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Rhi::Streaming
{
    struct BufferView
    {
        Buffer      RhiBuffer;
        BufferRange Range;

        explicit BufferView(
            Buffer      RhiBuffer,
            BufferRange Range = EntireBuffer) :
            RhiBuffer(std::move(RhiBuffer)),
            Range(Range.Transform(this->RhiBuffer))
        {
        }
    };
} // namespace Ame::Rhi::Streaming
