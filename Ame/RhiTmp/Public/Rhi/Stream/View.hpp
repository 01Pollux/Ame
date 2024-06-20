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
            Buffer&&    buffer,
            BufferRange range = c_EntireBuffer) :
            BufferRef(std::move(buffer)),
            Range(range.Transform(BufferRef))
        {
        }

        BufferView() = default;

        BufferView(const BufferView& other) :
            BufferRef(other.BufferRef.Borrow()),
            Range(other.Range)
        {
        }
        BufferView(BufferView&&) noexcept = default;

        BufferView& operator=(const BufferView& other)
        {
            if (this != &other)
            {
                BufferRef = other.BufferRef.Borrow();
                Range     = other.Range;
            }
            return *this;
        }
        BufferView& operator=(BufferView&&) noexcept = default;

        ~BufferView() = default;
    };
} // namespace Ame::Rhi::Streaming
