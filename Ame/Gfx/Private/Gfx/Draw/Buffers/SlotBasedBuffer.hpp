#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Stream/Buffer.hpp>

namespace Ame::Gfx::Draw::Buffers
{
    template<typename Ty>
    class SlotBasedBuffer
    {
    public:
        static constexpr uint32_t InitialInstanceCount = 1;
        static constexpr uint32_t SizePerInstance      = sizeof(Ty);

        SlotBasedBuffer(
            Rhi::Device& RhiDevice,
            uint32_t     InstanceCount = InitialInstanceCount) :
            m_InstanceCount(InstanceCount)
        {
            GrowSlots();
        }

    public:
        /// <summary>
        /// Flush the stream to the buffer
        /// </summary>
        void TryFlush()
        {
            if (m_Stream)
            {
                m_Stream.flush();
            }
        }

    public:
        /// <summary>
        /// Write data to the buffer
        /// </summary>
        [[nodiscard]] void Write(
            uint32_t  Index,
            const Ty& Data)
        {
            m_Stream.seekp(Index * SizePerInstance);
            m_Stream.write(std::bit_cast<const char*>(std::addressof(Data)), SizePerInstance);
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        uint32_t Rent(
            const Ty& Data)
        {
            if (m_EmptySlots.empty())
            {
                GrowSlots();
            }

            auto Slot = *m_EmptySlots.begin();
            m_EmptySlots.erase(Slot);

            Write(Slot, Data);
            return Slot;
        }

        /// <summary>
        /// Return a slot in the buffer
        /// </summary>
        void Return(
            uint32_t Slot)
        {
            m_EmptySlots.insert(Slot);
        }

    private:
        /// <summary>
        /// Grow the buffer to double the size
        /// </summary>
        void GrowSlots()
        {
            uint32_t FirstEmptySlot = m_EmptySlots.empty() ? m_InstanceCount : *m_EmptySlots.begin();
            m_InstanceCount *= 2;

            for (uint32_t i = FirstEmptySlot; i < m_InstanceCount; i++)
            {
                m_EmptySlots.insert(i);
            }

            RecreateBuffer();
        }

    private:
        /// <summary>
        /// Recreate the buffer and the stream
        /// </summary>
        void RecreateBuffer()
        {
            TryFlush();

            auto NewBuffer = CreateBuffer(m_InstanceCount);
            CopyToBuffer(NewBuffer);

            m_Stream = CreateOutputStream(NewBuffer);
            m_Buffer = std::move(NewBuffer);
        }

        /// <summary>
        /// Copy the current content of the buffer to the new buffer
        /// </summary>
        void CopyToBuffer(
            Rhi::Buffer& NewBuffer) const
        {
            if (m_Buffer)
            {
                auto Src  = m_Buffer.GetPtr();
                auto Dst  = NewBuffer.GetPtr();
                auto Size = m_Buffer.GetDesc().size;

                std::memcpy(Dst, Src, Size);
            }
        }

    private:
        /// <summary>
        /// Create an empty buffer
        /// </summary>
        static auto CreateBuffer(
            uint32_t InstanceCount)
        {
            Rhi::BufferDesc Desc{
                .size            = SizePerInstance * InstanceCount,
                .structureStride = SizePerInstance,
                .usageMask       = Rhi::BufferUsageBits::SHADER_RESOURCE
            };

            return Rhi::Buffer(
                m_RhiDevice,
                Rhi::MemoryLocation::HOST_UPLOAD,
                Desc);
        }

        /// <summary>
        /// Create the output stream for writing to the buffer
        /// </summary>
        [[nodiscard]] static auto CreateOutputStream(
            const Rhi::Buffer& Buffer)
        {
            return Rhi::Streaming::BufferOStream(Buffer);
        }

    private:
        Ref<Rhi::Device> m_RhiDevice;
        uint32_t         m_InstanceCount;

        Rhi::Buffer                   m_Buffer;
        Rhi::Streaming::BufferOStream m_Stream;

        // TODO: move this to a separate class for better encapsulation
        std::set<uint32_t> m_EmptySlots;
    };
} // namespace Ame::Gfx::Draw