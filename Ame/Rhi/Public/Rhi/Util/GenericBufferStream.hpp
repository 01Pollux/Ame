#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Stream/Buffer.hpp>

namespace Ame::Rhi::Util
{
    template<bool WithStreaming>
    class GenericBufferStream;

    template<>
    class GenericBufferStream<true>
    {
    public:
        GenericBufferStream() :
            m_Stream(std::make_unique<Streaming::BufferOStream>())
        {
        }

        GenericBufferStream(
            Buffer&& buffer) :
            m_Buffer(std::move(buffer)),
            m_Stream(std::make_unique<Streaming::BufferOStream>(Streaming::BufferView(m_Buffer)))
        {
        }

    public:
        /// <summary>
        /// Get the buffer
        /// </summary>
        /// <returns></returns>
        const Buffer& GetBuffer() const
        {
            return m_Buffer;
        }

        /// <summary>
        /// Get the size of buffer
        /// </summary>
        [[nodiscard]] size_t GetSize() const
        {
            return m_Buffer.GetDesc().size;
        }

    public:
        /// <summary>
        /// Open the buffer stream
        /// </summary>
        void Open(
            Buffer buffer)
        {
            m_Buffer = std::move(buffer);
            m_Stream->open(Streaming::BufferView(m_Buffer));
        }

        /// <summary>
        /// Check if the buffer stream is open
        /// </summary>
        [[nodiscard]] bool IsOpen() const
        {
            return m_Stream->is_open();
        }

        /// <summary>
        /// Flush the buffer stream
        /// </summary>
        void Flush()
        {
            m_Stream->flush();
        }

        /// <summary>
        /// Close the buffer stream
        /// </summary>
        void Close()
        {
            m_Stream->close();
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            size_t           offset,
            const std::byte* data,
            size_t           size)
        {
            m_Stream->seekp(offset);
            m_Stream->write(std::bit_cast<const char*>(data), size);
        }

        /// <summary>
        /// Copy the current content of the buffer to the new buffer
        /// </summary>
        void CopyTo(
            Buffer& newBuffer) const
        {
            if (m_Buffer)
            {
                auto src  = m_Buffer.GetPtr();
                auto dst  = newBuffer.GetPtr();
                auto size = m_Buffer.GetDesc().size;

                std::memcpy(dst, src, size);
            }
        }

    private:
        Buffer                         m_Buffer;
        UPtr<Streaming::BufferOStream> m_Stream;
    };

    template<>
    class GenericBufferStream<false>
    {
    public:
        GenericBufferStream() = default;
        GenericBufferStream(Buffer&& buffer) :
            m_Buffer(std::move(buffer))
        {
        }

    public:
        /// <summary>
        /// Get the buffer
        /// </summary>
        /// <returns></returns>
        const Buffer& GetBuffer() const
        {
            return m_Buffer;
        }

        /// <summary>
        /// Get the size of buffer
        /// </summary>
        [[nodiscard]] size_t GetSize() const
        {
            return m_Buffer.GetDesc().size;
        }

    public:
        /// <summary>
        /// Open the buffer stream
        /// </summary>
        void Open(
            Buffer buffer)
        {
            m_Buffer = std::move(buffer);
        }

        /// <summary>
        /// Check if the buffer stream is open
        /// </summary>
        [[nodiscard]] bool IsOpen() const
        {
            return true;
        }

        /// <summary>
        /// Flush the buffer stream
        /// </summary>
        void Flush()
        {
        }

        /// <summary>
        /// Close the buffer stream
        /// </summary>
        void Close()
        {
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            size_t           offset,
            const std::byte* data,
            size_t           size)
        {
            std::memcpy(m_Buffer.GetPtr(offset), data, size);
        }

        /// <summary>
        /// Copy the current content of the buffer to the new buffer
        /// </summary>
        void CopyTo(
            Buffer& newBuffer) const
        {
            if (m_Buffer)
            {
                auto src  = m_Buffer.GetPtr();
                auto dst  = newBuffer.GetPtr();
                auto size = m_Buffer.GetDesc().size;

                std::memcpy(dst, src, size);
            }
        }

    private:
        Buffer m_Buffer;
    };
} // namespace Ame::Rhi::Util