#include <Rhi/Stream/Sink.hpp>

namespace Ame::Rhi::Streaming
{
    BufferDevice::BufferDevice(
        BufferView view) :
        m_View(std::move(view)),
        m_GetPosition(m_View.Range.Offset),
        m_PutPosition(m_View.Range.Offset)
    {
    }

    auto BufferDevice::read(
        char_type*   data,
        streamsize_t size) -> streamsize_t
    {
        streamsize_t read = std::min(size, static_cast<streamsize_t>(m_View.Range.Size - m_GetPosition));
        if (read)
        {
            auto mapped = m_View.BufferRef.GetPtr(m_GetPosition);
            std::memcpy(data, mapped, read);
            m_GetPosition += read;
            return read;
        }
        else
        {
            return -1;
        }
    }

    auto BufferDevice::write(
        const char_type* data,
        streamsize_t     size) -> streamsize_t
    {
        if (m_PutPosition + size > m_View.Range.Offset + m_View.Range.Size)
        {
            return -1;
        }

        auto mapped = m_View.BufferRef.GetPtr(m_PutPosition);
        std::memcpy(mapped, data, size);

        m_PutPosition += size;
        return size;
    }

    auto BufferDevice::seek(
        streamoffset_t          offset,
        std::ios_base::seekdir  direction,
        std::ios_base::openmode mode) -> streamoffset_t
    {
        if (mode & std::ios_base::in)
        {
            streamoffset_t next;
            switch (direction)
            {
            case std::ios_base::beg:
                next = m_View.Range.Offset + offset;
                break;
            case std::ios_base::cur:
                next = m_GetPosition + offset;
                break;
            case std::ios_base::end:
                next = m_View.Range.Offset + m_View.Range.Size + offset - 1;
                break;
            default:
                std::unreachable();
            }

            if (next < m_View.Range.Offset || next >= m_View.Range.Offset + m_View.Range.Size)
            {
                throw std::ios_base::failure("Seek out of range.");
            }
            return m_GetPosition;
        }
        else if (mode & std::ios_base::out)
        {
            streamoffset_t next;
            switch (direction)
            {
            case std::ios_base::beg:
                next = m_View.Range.Offset + offset;
                break;
            case std::ios_base::cur:
                next = m_PutPosition + offset;
                break;
            case std::ios_base::end:
                next = m_View.Range.Offset + m_View.Range.Size + offset - 1;
                break;
            default:
                std::unreachable();
            }

            if (next < m_View.Range.Offset || next >= m_View.Range.Offset + m_View.Range.Size)
            {
                throw std::ios_base::failure("Seek out of range.");
            }
            return m_PutPosition;
        }
        else
        {
            return -1;
        }
    }
} // namespace Ame::Rhi::Streaming
