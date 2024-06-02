#pragma once

#include <boost/container/flat_set.hpp>

#include <Rhi/Device/Device.hpp>
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
    template<typename Ty>
    class SlotBasedBuffer
    {
    public:
        using Type                                         = Ty;
        static constexpr uint32_t c_AlignedSizePerInstance = Math::AlignUp(sizeof(Type), 16);
        static constexpr uint32_t c_InvalidIndex           = std::numeric_limits<uint32_t>::max();
        static constexpr uint64_t c_InvalidValue64         = std::numeric_limits<uint64_t>::max();

    private:
        struct Handle
        {
            uint64_t FrameIndex = c_InvalidValue64;
            uint32_t Slot       = c_InvalidIndex;

            [[nodiscard]] auto operator<=>(
                const Handle& other) const
            {
                return FrameIndex <=> other.FrameIndex;
            }
        };

        using EmptySlotSet     = boost::container::flat_set<uint32_t>;
        using BusyHandleSet    = boost::container::flat_set<uint32_t>;
        using RetiredHandleSet = boost::container::flat_multiset<Handle>;
        using BufferStream     = UPtr<Streaming::BufferOStream>;

    public:
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
            uint32_t    slot,
            const Type& data)
        {
            AllocationMustExists(slot);
            Write(slot, std::addressof(data), sizeof(Type));
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t    slot,
            const void* data,
            size_t      size)
        {
            AllocationMustExists(slot);
            size_t offset = GetOffset(slot);
            m_Stream->seekp(offset);
            m_Stream->write(static_cast<const char*>(data), size);
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
            return m_Buffer;
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] uint32_t Rent()
        {
            uint64_t currentFrameIndex = m_Device.get().GetFrameCount();
            DiscardRetiredHandles(currentFrameIndex);

            if (m_EmptySlots.empty())
            {
                GrowSlots();
            }

            auto slot = *m_EmptySlots.begin();
            m_EmptySlots.erase(slot);
            RentSlot(slot);

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
            AllocationMustExists(slot);
            RetireHandle(slot, m_Device.get().GetFrameIndex());
        }

    public:
        /// <summary>
        /// Reset all slots in the buffer
        /// </summary>
        void Reset()
        {
            uint64_t currentFrameIndex = m_Device.get().GetFrameCount();
            RetireAllSlots(currentFrameIndex);
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
        /// Rent a slot in the buffer
        /// </summary>
        void RentSlot(
            uint32_t slot)
        {
            m_BusySlots.insert(slot);
        }

        /// <summary>
        /// Rent all handles in the buffer
        /// </summary>
        void RetireAllSlots(
            uint64_t frameIndex)
        {
            for (uint32_t i : m_BusySlots)
            {
                m_RetiredHandles.insert({ .FrameIndex = frameIndex, .Slot = i });
            }
            m_BusySlots.clear();
        }

        /// <summary>
        /// Retire a handle in the buffer
        /// </summary>
        void RetireHandle(
            uint32_t slot,
            uint64_t frameIndex)
        {
            m_BusySlots.erase(slot);
            m_RetiredHandles.insert({ .FrameIndex = frameIndex, .Slot = slot });
        }

        /// <summary>
        /// Flush all frames that are older than the given frame index + frame count
        /// </summary>
        void DiscardRetiredHandles(
            uint64_t currentFrameIndex)
        {
            uint8_t frameCount = m_Device.get().GetFrameCountInFlight();
            if (frameCount > currentFrameIndex) [[unlikely]]
            {
                return;
            }

            currentFrameIndex -= frameCount;

            auto it = m_RetiredHandles.begin();
            while (it != m_RetiredHandles.end())
            {
                if (it->FrameIndex <= currentFrameIndex)
                {
                    m_EmptySlots.insert(it->Slot);
                    it = m_RetiredHandles.erase(it);
                }
                else
                {
                    break;
                }
            }
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
            if (m_BusySlots.contains(slot))
            {
                std::unreachable();
            }
#endif
        }

    private:
        Ref<Device>         m_Device;
        SlotBasedBufferDesc m_Desc;

        Buffer       m_Buffer;
        BufferStream m_Stream;

        EmptySlotSet     m_EmptySlots;
        BusyHandleSet    m_BusySlots;
        RetiredHandleSet m_RetiredHandles;
    };
} // namespace Ame::Rhi::Util