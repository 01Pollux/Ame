#pragma once

#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    class DescriptorSet
    {

    public:
        DescriptorSet() = default;
        DescriptorSet(std::nullptr_t)
        {
        }

        DescriptorSet(
            DeviceResourceAllocator& allocator,
            nri::DescriptorSet*      set);

    public:
        [[nodiscard]] auto operator<=>(
            const DescriptorSet& other) const noexcept
        {
            return m_Set <=> other.m_Set;
        }

        [[nodiscard]] bool operator==(
            const DescriptorSet& other) const noexcept
        {
            return m_Set == other.m_Set;
        }

        explicit operator bool() const noexcept
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
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri descriptor table.
        /// </summary>
        [[nodiscard]] nri::DescriptorSet* const& Unwrap() const;

    private:
        DeviceResourceAllocator* m_Allocator = nullptr;
        nri::DescriptorSet*      m_Set       = nullptr;
    };
} // namespace Ame::Rhi