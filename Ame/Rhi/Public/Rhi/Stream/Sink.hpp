#pragma once

#include <Rhi/Stream/Device.hpp>

namespace Ame::Rhi::Streaming
{
    class BufferSink : public boost::iostreams::device<boost::iostreams::output_seekable>
    {
    public:
        using streamsize_t   = BufferDevice::streamsize_t;
        using streamoffset_t = BufferDevice::streamoffset_t;
        using char_type      = BufferDevice::char_type;

        BufferSink(
            BufferView view) :
            m_Device(std::move(view))
        {
        }

        streamsize_t write(
            const char_type* data,
            streamsize_t     size)
        {
            return m_Device.write(data, size);
        }

        streamoffset_t seek(
            streamoffset_t         offset,
            std::ios_base::seekdir direction)
        {
            return m_Device.seek(offset, direction, std::ios::out);
        }

    public:
        /// <summary>
        /// Get the buffer.
        /// </summary>
        const Buffer& GetBuffer() const
        {
            return m_Device.GetBuffer();
        }

        /// <summary>
        /// Get the buffer range.
        /// </summary>
        const BufferRange& GetRange() const
        {
            return m_Device.GetRange();
        }

    private:
        BufferDevice m_Device;
    };
} // namespace Ame::Rhi::Streaming
