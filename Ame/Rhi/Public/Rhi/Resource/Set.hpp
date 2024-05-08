#pragma once

#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class DescriptorSet
    {
    public:
        DescriptorSet() = default;

        DescriptorSet(
            Rhi::DeviceImpl*    RhiDevice,
            nri::DescriptorSet* Set) :
            m_RhiDevice(RhiDevice),
            m_Set(Set)
        {
        }

        operator bool() const noexcept
        {
            return m_Set != nullptr;
        }

    public:
        /// <summary>
        /// Set the descriptor ranges.
        /// </summary>
        void SetRanges(
            uint32_t                                   BaseRange,
            std::span<const DescriptorRangeUpdateDesc> RangeUpdateDescs);

        /// <summary>
        /// Set the descriptor ranges.
        /// </summary>
        void SetRange(
            uint32_t                         BaseRange,
            const DescriptorRangeUpdateDesc& RangeUpdateDesc);

        /// <summary>
        /// Set the dynamic buffers.
        /// </summary>
        void SetDynamicBuffers(
            uint32_t                          BufferOffset,
            std::span<const nri::Descriptor*> Buffers);

        /// <summary>
        /// Set the dynamic buffers.
        /// </summary>
        void SetDynamicBuffer(
            uint32_t               BufferOffset,
            const nri::Descriptor* Buffer);

        /// <summary>
        /// Copy the descriptor set.
        /// </summary>
        void CopyTo(
            const DescriptorSetCopyDesc& CopyDesc);

    public:
        /// <summary>
        /// Get the nri descriptor set.
        /// </summary>
        [[nodiscard]] nri::DescriptorSet* Unwrap() const;

    private:
        DeviceImpl*         m_RhiDevice = nullptr;
        nri::DescriptorSet* m_Set       = nullptr;
    };
} // namespace Ame::Rhi