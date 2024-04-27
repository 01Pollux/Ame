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
            BufferView View) :
            m_Device(View)
        {
        }

        streamsize_t write(
            const char_type* Data,
            streamsize_t     Size)
        {
            return m_Device.write(Data, Size);
        }

        streamoffset_t seek(
            streamoffset_t         Offset,
            std::ios_base::seekdir Direction)
        {
            return m_Device.seek(Offset, Direction, std::ios::out);
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
