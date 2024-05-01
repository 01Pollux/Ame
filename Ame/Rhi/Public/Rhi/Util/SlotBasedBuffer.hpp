#pragma once

#include <set>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Stream/Buffer.hpp>
#include <Math/Common.hpp>

namespace Ame::Rhi::Util
{
    struct SlotBasedBufferDesc
    {
        /// <summary>
        /// Initial number of instances in the buffer
        /// </summary>
        uint32_t InstanceCount = 1;

        /// <summary>
        /// Alignment of the instances in the buffer
        /// </summary>
        uint32_t Alignment = 1;

        /// <summary>
        /// Maximum number of instances in the buffer
        /// </summary>
        uint32_t MaxInstances = 0;

        /// <summary>
        /// Grow factor of the buffer
        /// </summary>
        float GrowFactor = 2.0f;

        /// <summary>
        /// Usage flags of the buffer
        /// </summary>
        Rhi::BufferUsageBits UsageFlags = Rhi::BufferUsageBits::SHADER_RESOURCE;
    };

    /// <summary>
    /// Slot based buffer for storing data in a buffer
    /// Each slot is a fixed size and can be rented and returned
    /// A buffer is continuously grown when all slots are rented
    /// </summary>
    template<typename Ty>
    class SlotBasedBuffer
    {
    public:
        static constexpr uint32_t SizePerInstance = sizeof(Ty);

        SlotBasedBuffer(
            Rhi::Device&        RhiDevice,
            SlotBasedBufferDesc Desc = {}) :
            m_Device(RhiDevice),
            m_Desc(Desc)
        {
            GrowSlots(m_Desc.InstanceCount);
        }

    public:
        /// <summary>
        /// Flush the stream to the buffer
        /// </summary>
        void Flush()
        {
            m_Stream.flush();
        }

    public:
        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t  Index,
            const Ty& Data)
        {
            Write(Index, std::addressof(Data), SizePerInstance);
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t    Index,
            const void* Data,
            size_t      Size)
        {
            size_t Offset = GetOffset(Index);
            m_Stream.seekp(Offset);
            m_Stream.write(static_cast<const char*>(Data), Size);
        }

    public:
        /// <summary>
        /// Get the offset of the slot in the buffer
        /// </summary>
        [[nodiscard]] size_t GetOffset(
            uint32_t Index) const
        {
            return Math::AlignUp(static_cast<size_t>(Index) * SizePerInstance, m_Desc.Alignment);
        }

        /// <summary>
        /// Get the number of empty slots in the buffer
        /// </summary>
        [[nodiscard]] size_t GetEmptyCount() const
        {
            return m_EmptySlots.size();
        }
        
        /// <summary>
        /// Get the number of allocated slots in the buffer
        /// </summary>
        [[nodiscard]] size_t GetAllocatedCount() const
        {
            return m_Desc.InstanceCount - m_EmptySlots.size();
        }

    public:
        /// <summary>
        /// Get buffer of the slot based buffer
        /// </summary>
        [[nodiscard]] const Rhi::Buffer& GetBuffer() const
        {
            return m_Buffer;
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] uint32_t Rent(
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
        /// Grow the buffer by the grow factor
        /// </summary>
        void GrowSlots()
        {
            GrowSlots(static_cast<uint32_t>(m_Desc.InstanceCount * m_Desc.GrowFactor));
        }

        /// <summary>
        /// Grow the buffer by the specified number of slots
        /// </summary>
        void GrowSlots(
            uint32_t InstanceCount)
        {
            if (m_Desc.MaxInstances != 0)
            {
                InstanceCount = std::min(InstanceCount, m_Desc.MaxInstances);
            }

            uint32_t FirstEmptySlot = m_EmptySlots.empty() ? m_Desc.InstanceCount : *m_EmptySlots.begin();
            m_Desc.InstanceCount    = InstanceCount;

            for (uint32_t i = FirstEmptySlot; i < m_Desc.InstanceCount; i++)
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
            if (m_Stream)
            {
                Flush();
                m_Stream.close();
            }

            auto NewBuffer = CreateBuffer(m_Desc.InstanceCount);
            CopyToBuffer(NewBuffer);

            m_Buffer = std::move(NewBuffer);
            m_Stream.open(m_Buffer);
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
        [[nodiscard]] auto CreateBuffer(
            uint32_t InstanceCount)
        {
            Rhi::BufferDesc Desc{
                .size            = SizePerInstance * InstanceCount,
                .structureStride = SizePerInstance,
                .usageMask       = m_Desc.UsageFlags,
            };

            return Rhi::Buffer(
                m_Device.get(),
                Rhi::MemoryLocation::HOST_UPLOAD,
                Desc);
        }

    private:
        Ref<Rhi::Device>    m_Device;
        SlotBasedBufferDesc m_Desc;

        Rhi::Buffer                   m_Buffer;
        Rhi::Streaming::BufferOStream m_Stream;

        std::set<uint32_t> m_EmptySlots;
    };
} // namespace Ame::Rhi::Util