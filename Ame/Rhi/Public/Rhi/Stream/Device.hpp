#pragma once

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/concepts.hpp>

#include <Rhi/Stream/View.hpp>

namespace Ame::Rhi::Streaming
{
    class BufferDevice : public boost::iostreams::device<boost::iostreams::dual_seekable>
    {
    public:
        using streamsize_t   = std::streamsize;
        using streamoffset_t = boost::iostreams::stream_offset;
        using char_type      = char;

        BufferDevice(
            BufferView View);

        streamsize_t read(
            char_type*   Data,
            streamsize_t Size);

        streamsize_t write(
            const char_type* Data,
            streamsize_t     Size);

        streamoffset_t seek(
            streamoffset_t          Offset,
            std::ios_base::seekdir  Direction,
            std::ios_base::openmode Mode);

    public:
        /// <summary>
        /// Get the buffer.
        /// </summary>
        const Buffer& GetBuffer() const
        {
            return m_View.RhiBuffer;
        }

        /// <summary>
        /// Get the buffer range.
        /// </summary>
        const BufferRange& GetRange() const
        {
            return m_View.Range;
        }

    private:
        BufferView     m_View;
        streamoffset_t m_GetPosition = 0;
        streamoffset_t m_PutPosition = 0;
    };
} // namespace Ame::Rhi::Streaming
