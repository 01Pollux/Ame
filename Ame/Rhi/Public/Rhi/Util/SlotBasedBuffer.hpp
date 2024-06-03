#pragma once

#include <boost/container/flat_set.hpp>
#include <Rhi/Util/GenericBufferStream.hpp>
#include <Math/Common.hpp>

namespace Ame::Rhi::Util
{
    struct SlotBasedBufferDesc
    {
        /// <summary>
        /// Initial number of instances in the buffer
        /// </summary>
        uint32_t InstanceCount = 1'024;

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
        BufferUsageBits UsageFlags = BufferUsageBits::SHADER_RESOURCE;

        /// <summary>
        /// Memory location of the buffer
        /// </summary>
        MemoryLocation Location = MemoryLocation::HOST_UPLOAD;
    };

    /// <summary>
    /// Slot based buffer for storing data in a buffer
    /// Each slot is a fixed size and can be rented and returned
    /// A buffer is continuously grown when all slots are rented
    /// </summary>
    template<typename Ty, bool WithStreaming = true>
    class SlotBasedBuffer
    {
    public:
        using Type                                         = Ty;
        static constexpr uint32_t c_AlignedSizePerInstance = Math::AlignUp(sizeof(Type), 16);
        static constexpr uint32_t c_InvalidIndex           = std::numeric_limits<uint32_t>::max();

    private:
        using EmptySlotSet  = boost::container::flat_set<uint32_t>;
        using BusyHandleSet = boost::container::flat_set<uint32_t>;
        using BufferStream  = GenericBufferStream<WithStreaming>;

    public:
        SlotBasedBuffer(
            Device&                    rhiDevice,
            const SlotBasedBufferDesc& desc = {}) :
            m_Device(rhiDevice),
            m_Desc(desc)
        {
            uint32_t InitialCount = std::exchange(m_Desc.InstanceCount, 0);
            GrowSlots(InitialCount);
        }

    public:
        /// <summary>
        /// Flush the stream to the buffer
        /// </summary>
        void Flush()
        {
            m_BufferStream.Flush();
        }

    public:
        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t    slot,
            const Type& data)
        {
            AllocationMustExists(slot);
            Write(slot, std::bit_cast<const std::byte*>(std::addressof(data)), sizeof(Type));
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t         slot,
            const std::byte* data,
            size_t           size)
        {
            AllocationMustExists(slot);
            size_t offset = GetOffset(slot);
            m_BufferStream.Write(offset, data, size);
        }

    public:
        /// <summary>
        /// Get the offset of the slot in the buffer
        /// </summary>
        [[nodiscard]] size_t GetOffset(
            uint32_t slot) const
        {
            AllocationMustExists(slot);
            return Math::AlignUp(static_cast<size_t>(slot) * c_AlignedSizePerInstance, m_Desc.Alignment);
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
        [[nodiscard]] const Buffer& GetBuffer() const
        {
            return m_BufferStream.GetBuffer();
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] uint32_t Rent()
        {
            if (m_EmptySlots.empty())
            {
                GrowSlots();
            }

            uint32_t slot = *m_EmptySlots.begin();
            m_EmptySlots.erase(slot);

            return slot;
        }

        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        uint32_t Rent(
            const Type& data)
        {
            uint32_t slot = Rent();
            Write(slot, data);
            return slot;
        }

        /// <summary>
        /// Return a slot in the buffer
        /// </summary>
        void Return(
            uint32_t slot)
        {
            AllocationMustExists(slot);
            m_EmptySlots.insert(slot);
        }

    public:
        /// <summary>
        /// Reset all slots in the buffer
        /// </summary>
        void Reset()
        {
            for (uint32_t i = 0; i < m_Desc.InstanceCount; i++)
            {
                m_EmptySlots.insert(i);
            }
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
            uint32_t instanceCount)
        {
            if (m_Desc.MaxInstances != 0)
            {
                instanceCount = std::min(instanceCount, m_Desc.MaxInstances);
            }

            uint32_t firstEmptySlot = m_EmptySlots.empty() ? m_Desc.InstanceCount : *m_EmptySlots.begin();
            m_Desc.InstanceCount    = instanceCount;

            for (uint32_t i = firstEmptySlot; i < m_Desc.InstanceCount; i++)
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
            if (m_BufferStream.IsOpen())
            {
                Flush();
                m_BufferStream.Close();
            }

            auto newBuffer = CreateBuffer(m_Desc.InstanceCount);
            m_BufferStream.CopyTo(newBuffer);

            m_BufferStream.Open(std::move(newBuffer));
        }

    private:
        /// <summary>
        /// Create an empty buffer
        /// </summary>
        [[nodiscard]] auto CreateBuffer(
            uint32_t instanceCount) const
        {
            BufferDesc desc{
                .size            = c_AlignedSizePerInstance * instanceCount,
                .structureStride = c_AlignedSizePerInstance,
                .usageMask       = m_Desc.UsageFlags,
            };

            return Buffer(
                m_Device.get(),
                m_Desc.Location,
                desc);
        }

    private:
        void AllocationMustExists(
            uint32_t slot) const
        {
#ifdef AME_DEBUG
            if (slot >= m_Desc.InstanceCount ||
                !m_EmptySlots.contains(slot))
            {
                std::unreachable();
            }
#endif
        }

    private:
        Ref<Device>         m_Device;
        SlotBasedBufferDesc m_Desc;

        BufferStream m_BufferStream;
        EmptySlotSet m_EmptySlots;
    };
} // namespace Ame::Rhi::Util