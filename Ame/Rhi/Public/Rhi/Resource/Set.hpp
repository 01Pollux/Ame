#pragma once

#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class DescriptorSet
    {
    public:
        DescriptorSet() = default;

        DescriptorSet(
            nri::DescriptorSet* Set) :
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
            Device&                                    RhiDevice,
            uint32_t                                   BaseRange,
            std::span<const DescriptorRangeUpdateDesc> RangeUpdateDescs);

        /// <summary>
        /// Set the descriptor ranges.
        /// </summary>
        void SetRange(
            Device&                          RhiDevice,
            uint32_t                         BaseRange,
            const DescriptorRangeUpdateDesc& RangeUpdateDesc);

        /// <summary>
        /// Set the dynamic buffers.
        /// </summary>
        void SetDynamicBuffers(
            Device&                           RhiDevice,
            uint32_t                          BaseBuffer,
            std::span<const nri::Descriptor*> Buffers);

        /// <summary>
        /// Set the dynamic buffers.
        /// </summary>
        void SetDynamicBuffer(
            Device&                RhiDevice,
            uint32_t               BaseBuffer,
            const nri::Descriptor* Buffer);

        /// <summary>
        /// Copy the descriptor set.
        /// </summary>
        void CopyTo(
            Device&                      RhiDevice,
            const DescriptorSetCopyDesc& CopyDesc);

    public:
        /// <summary>
        /// Get the nri descriptor set.
        /// </summary>
        [[nodiscard]] nri::DescriptorSet* Unwrap() const;

    private:
        nri::DescriptorSet* m_Set = nullptr;
    };
} // namespace Ame::Rhi