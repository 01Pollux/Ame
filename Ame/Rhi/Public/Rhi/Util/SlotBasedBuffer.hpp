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
        using Type                                  = Ty;
        static constexpr uint32_t c_SizePerInstance = sizeof(Type);
        static constexpr uint32_t c_InvalidIndex    = std::numeric_limits<uint32_t>::max();

        SlotBasedBuffer(
            Device&                    rhiDevice,
            const SlotBasedBufferDesc& desc = {}) :
            m_Device(rhiDevice),
            m_Stream(std::make_unique<Streaming::BufferOStream>()),
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
            m_Stream->flush();
        }

    public:
        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t    index,
            const Type& data)
        {
            Write(index, std::addressof(data), c_SizePerInstance);
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t    index,
            const void* data,
            size_t      size)
        {
            size_t offset = GetOffset(index);
            m_Stream->seekp(offset);
            m_Stream->write(static_cast<const char*>(data), size);
        }

    public:
        /// <summary>
        /// Get the offset of the slot in the buffer
        /// </summary>
        [[nodiscard]] size_t GetOffset(
            uint32_t index) const
        {
            return Math::AlignUp(static_cast<size_t>(index) * c_SizePerInstance, m_Desc.Alignment);
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
            return m_Buffer;
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

            auto slot = *m_EmptySlots.begin();
            m_EmptySlots.erase(slot);

            return slot;
        }

        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        uint32_t Rent(
            const Type& data)
        {
            auto slot = Rent();
            Write(slot, data);
            return slot;
        }

        /// <summary>
        /// Return a slot in the buffer
        /// </summary>
        void Return(
            uint32_t slot)
        {
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
            if (m_Stream->is_open())
            {
                Flush();
                m_Stream->close();
            }

            auto newBuffer = CreateBuffer(m_Desc.InstanceCount);
            CopyToBuffer(newBuffer);

            m_Buffer = std::move(newBuffer);
            m_Stream->open(Streaming::BufferView(m_Buffer));
        }

        /// <summary>
        /// Copy the current content of the buffer to the new buffer
        /// </summary>
        void CopyToBuffer(
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
        /// <summary>
        /// Create an empty buffer
        /// </summary>
        [[nodiscard]] auto CreateBuffer(
            uint32_t instanceCount)
        {
            BufferDesc desc{
                .size            = c_SizePerInstance * instanceCount,
                .structureStride = c_SizePerInstance,
                .usageMask       = m_Desc.UsageFlags,
            };

            return Buffer(
                m_Device.get(),
                MemoryLocation::HOST_UPLOAD,
                desc);
        }

    private:
        Ref<Device>         m_Device;
        SlotBasedBufferDesc m_Desc;

        Buffer                         m_Buffer;
        UPtr<Streaming::BufferOStream> m_Stream;

        std::set<uint32_t> m_EmptySlots;
    };
} // namespace Ame::Rhi::Util