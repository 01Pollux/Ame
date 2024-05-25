#pragma once

#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class DescriptorSet
    {
    public:
        DescriptorSet() = default;

        DescriptorSet(
            DeviceImpl*         rhiDeviceImpl,
            nri::DescriptorSet* nriSet) :
            m_RhiDevice(rhiDeviceImpl),
            m_Set(nriSet)
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
            uint32_t                                   baseRange,
            std::span<const DescriptorRangeUpdateDesc> rangeUpdateDescs);

        /// <summary>
        /// Set the descriptor ranges.
        /// </summary>
        void SetRange(
            uint32_t                         baseRange,
            const DescriptorRangeUpdateDesc& rangeUpdateDesc);

        /// <summary>
        /// Set the dynamic buffers.
        /// </summary>
        void SetDynamicBuffers(
            uint32_t                          baseBufferInSet,
            std::span<const nri::Descriptor*> buffers);

        /// <summary>
        /// Set the dynamic buffers.
        /// </summary>
        void SetDynamicBuffer(
            uint32_t               baseBufferInSet,
            const nri::Descriptor* buffer);

        /// <summary>
        /// Copy the descriptor set.
        /// </summary>
        void CopyTo(
            const DescriptorSetCopyDesc& copyDesc);

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