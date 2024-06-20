#pragma once

#include <Rhi/Core.hpp>
#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Descs/View.hpp>

namespace Ame::Rhi::Streaming
{
    struct BufferView
    {
        Buffer      BufferRef;
        BufferRange Range;

        BufferView() = default;
        explicit BufferView(
            Buffer      buffer,
            BufferRange range = c_EntireBuffer) :
            BufferRef(std::move(buffer)),
            Range(range.Transform(BufferRef))
        {
        }
    };
} // namespace Ame::Rhi::Streaming
