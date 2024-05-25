#pragma once

#include <Rhi/Core.hpp>
#include <Rhi/Resource/View.hpp>
#include <Rhi/Resource/Buffer.hpp>

namespace Ame::Rhi::Streaming
{
    struct BufferView
    {
        Buffer      BufferRef;
        BufferRange Range;

        explicit BufferView(
            Buffer      buffer,
            BufferRange range = EntireBuffer) :
            BufferRef(std::move(buffer)),
            Range(range.Transform(BufferRef))
        {
        }
    };
} // namespace Ame::Rhi::Streaming
