#include <Rhi/Stream/Sink.hpp>

namespace Ame::Rhi::Streaming
{
    BufferDevice::BufferDevice(
        BufferView View) :
        m_View(std::move(View)),
        m_GetPosition(m_View.Range.Offset),
        m_PutPosition(m_View.Range.Offset)
    {
    }

    auto BufferDevice::read(
        char_type*   Data,
        streamsize_t Size) -> streamsize_t
    {
        streamsize_t Read = std::min(Size, static_cast<streamsize_t>(m_View.Range.Size - m_GetPosition));
        if (Read)
        {
            auto Mapped = m_View.RhiBuffer.GetPtr(m_GetPosition);
            std::memcpy(Data, Mapped, Read);
            m_GetPosition += Read;
            return Read;
        }
        else
        {
            return -1;
        }
    }

    auto BufferDevice::write(
        const char_type* Data,
        streamsize_t     Size) -> streamsize_t
    {
        if (m_PutPosition + Size > m_View.Range.Offset + m_View.Range.Size)
        {
            return -1;
        }

        auto Mapped = m_View.RhiBuffer.GetPtr(m_PutPosition);
        std::memcpy(Mapped, Data, Size);

        m_PutPosition += Size;
        return Size;
    }

    auto BufferDevice::seek(
        streamoffset_t          Offset,
        std::ios_base::seekdir  Direction,
        std::ios_base::openmode Mode) -> streamoffset_t
    {
        if (Mode & std::ios_base::in)
        {
            streamoffset_t Next;
            switch (Direction)
            {
            case std::ios_base::beg:
                Next = m_View.Range.Offset + Offset;
                break;
            case std::ios_base::cur:
                Next = m_GetPosition + Offset;
                break;
            case std::ios_base::end:
                Next = m_View.Range.Offset + m_View.Range.Size + Offset - 1;
                break;
            default:
                std::unreachable();
            }

            if (Next < m_View.Range.Offset || Next >= m_View.Range.Offset + m_View.Range.Size)
            {
                throw std::ios_base::failure("Seek out of range.");
            }
            return m_GetPosition;
        }
        else if (Mode & std::ios_base::out)
        {
            streamoffset_t Next;
            switch (Direction)
            {
            case std::ios_base::beg:
                Next = m_View.Range.Offset + Offset;
                break;
            case std::ios_base::cur:
                Next = m_PutPosition + Offset;
                break;
            case std::ios_base::end:
                Next = m_View.Range.Offset + m_View.Range.Size + Offset - 1;
                break;
            default:
                std::unreachable();
            }

            if (Next < m_View.Range.Offset || Next >= m_View.Range.Offset + m_View.Range.Size)
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
