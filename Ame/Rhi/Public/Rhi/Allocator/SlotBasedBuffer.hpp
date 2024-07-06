#pragma once

#include <boost/container/flat_set.hpp>
#include <Math/Common.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Rhi::Util
{
    struct SlotBasedBufferDesc
    {
        Dg::IRenderDevice*                Device = nullptr;
        Dg::BufferDesc                    BuffDesc;
        uint32_t                          InstanceCount  = 1'024;
        uint32_t                          Alignment      = 1;
        float                             GrowFactor     = 2.0f;
        std::function<void(Dg::IBuffer*)> OnBufferCreate = nullptr;
        uint32_t                          MaxInstances   = 0;
    };

    /// <summary>
    /// Slot based buffer for storing data in a buffer
    /// Each slot is a fixed size and can be rented and returned
    /// A buffer is continuously grown when all slots are rented
    /// </summary>
    // template<typename Ty>
    //     requires std::is_standard_layout_v<Ty> // must be standard layout
    using Ty = uint32_t;
    class SlotBasedBuffer
    {
    public:
        using DescType                              = SlotBasedBufferDesc;
        using SlotType                              = uint32_t;
        using Handle                                = SlotType;
        using DataType                              = Ty;
        static constexpr uint32_t c_SizePerInstance = sizeof(DataType);
        static constexpr uint32_t c_InvalidSlot     = std::numeric_limits<SlotType>::max();

    private:
        using EmptySlotSet = boost::container::flat_set<SlotType>;

    public:
        SlotBasedBuffer(
            const SlotBasedBufferDesc& desc = {}) :
            m_Desc(desc)
        {
        }

    public:
        /// <summary>
        /// Get the offset of the slot in the buffer
        /// </summary>
        [[nodiscard]] size_t GetOffset(
            SlotType slot) const
        {
            AllocationMustExists(slot);
            return Math::AlignUp(static_cast<size_t>(slot) * c_SizePerInstance, m_Desc.Alignment);
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
        [[nodiscard]] const Dg::Ptr<Dg::IBuffer>& GetBuffer() const
        {
            return m_Buffer;
        }

        /// <summary>
        /// Get desc of the slot based buffer
        /// </summary>
        [[nodiscard]] const auto& GetDesc() const
        {
            return m_Desc;
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] SlotType Rent(
            Dg::IDeviceContext* context)
        {
            if (m_EmptySlots.empty())
            {
                GrowSlots(context);
            }

            SlotType slot = *m_EmptySlots.begin();
            m_EmptySlots.erase(slot);

            return slot;
        }

        /// <summary>
        /// Return a slot in the buffer
        /// </summary>
        void Return(
            SlotType slot)
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
            for (SlotType i = 0; i < m_Desc.InstanceCount; i++)
            {
                m_EmptySlots.insert(i);
            }
        }

    private:
        /// <summary>
        /// Grow the buffer by the grow factor
        /// </summary>
        void GrowSlots(
            Dg::IDeviceContext* context)
        {
            // if buffer is null (first time) don't grow the buffer
            float factor = m_Buffer ? m_Desc.GrowFactor : 1.f;
            GrowSlots(context, static_cast<SlotType>(m_Desc.InstanceCount * factor));
        }

        /// <summary>
        /// Grow the buffer by the specified number of slots
        /// </summary>
        void GrowSlots(
            Dg::IDeviceContext* context,
            uint32_t            instanceCount)
        {
            if (m_Desc.MaxInstances != 0)
            {
                instanceCount = std::min(instanceCount, m_Desc.MaxInstances);
            }

            SlotType firstEmptySlot = m_EmptySlots.empty() ? m_Desc.InstanceCount : *m_EmptySlots.begin();
            m_Desc.InstanceCount    = instanceCount;

            for (SlotType i = firstEmptySlot; i < m_Desc.InstanceCount; i++)
            {
                m_EmptySlots.insert(i);
            }

            RecreateBuffer(context);
        }

    private:
        /// <summary>
        /// Recreate the buffer and the stream
        /// </summary>
        void RecreateBuffer(
            Dg::IDeviceContext* context)
        {
            auto buffer = std::move(m_Buffer);

            Dg::BufferData bufferData;

            if (buffer)
            {
                Dg::PVoid mappedData;
                context->MapBuffer(buffer, Dg::MAP_READ, Dg::MAP_FLAG_NONE, mappedData);

                bufferData.pData    = mappedData;
                bufferData.DataSize = m_Desc.BuffDesc.Size;
                bufferData.pContext = context;
            }

            m_Desc.Device->CreateBuffer(
                m_Desc.BuffDesc,
                buffer ? &bufferData : nullptr,
                &m_Buffer);

            if (buffer)
            {
                context->UnmapBuffer(buffer, Dg::MAP_READ);
            }
        }

    private:
        void AllocationMustExists(
            SlotType slot) const
        {
#ifdef AME_DEBUG
            if (slot >= m_Desc.InstanceCount ||
                m_EmptySlots.contains(slot))
            {
                throw std::out_of_range("Slot does not exist");
                std::unreachable();
            }
#endif
        }

    private:
        SlotBasedBufferDesc  m_Desc;
        Dg::Ptr<Dg::IBuffer> m_Buffer;
        EmptySlotSet         m_EmptySlots;
    };
} // namespace Ame::Rhi::Util