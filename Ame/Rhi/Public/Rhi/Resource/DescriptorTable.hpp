#pragma once

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Resource/ScopedResource.hpp>
#include <Rhi/Resource/DescriptorSet.hpp>

namespace Ame::Rhi
{
    class DescriptorTable
    {
    public:
        DescriptorTable() = default;
        DescriptorTable(std::nullptr_t)
        {
        }

        DescriptorTable(
            DeviceResourceAllocator& allocator,
            nri::DescriptorPool*     descriptorPool);

        DescriptorTable(const DescriptorTable&) = default;
        DescriptorTable(DescriptorTable&& other) noexcept :
            m_Allocator(std::exchange(other.m_Allocator, nullptr)),
            m_DescriptorPool(std::exchange(other.m_DescriptorPool, nullptr))
        {
        }

        DescriptorTable& operator=(const DescriptorTable&) = default;
        DescriptorTable& operator=(DescriptorTable&& other) noexcept
        {
            if (this != &other)
            {
                m_Allocator      = std::exchange(other.m_Allocator, nullptr);
                m_DescriptorPool = std::exchange(other.m_DescriptorPool, nullptr);
            }
            return *this;
        }

        ~DescriptorTable() = default;

    public:
        void Release(
            bool defer = true);

    public:
        [[nodiscard]] auto operator<=>(
            const DescriptorTable& other) const noexcept
        {
            return m_DescriptorPool <=> other.m_DescriptorPool;
        }

        [[nodiscard]] bool operator==(
            const DescriptorTable& other) const noexcept
        {
            return m_DescriptorPool == other.m_DescriptorPool;
        }

        explicit operator bool() const noexcept
        {
            return m_DescriptorPool != nullptr;
        }

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri descriptor table.
        /// </summary>
        [[nodiscard]] nri::DescriptorPool* const& Unwrap() const;

    public:
        /// <summary>
        /// Reset the descriptor pool and invalidates all descriptor sets allocated from it.
        /// </summary>
        void Reset();

        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] DescriptorSet AllocateSet(
            const nri::PipelineLayout& layout,
            uint32_t                   layoutSlot,
            uint32_t                   variableCount = 0);

        /// <summary>
        /// Allocate descriptor sets for the pipeline layout.
        /// </summary>
        [[nodiscard]] std::vector<DescriptorSet> AllocateSets(
            const nri::PipelineLayout& layout,
            uint32_t                   layoutSlot,
            uint32_t                   instanceCount = 1,
            uint32_t                   variableCount = 0);

    private:
        DeviceResourceAllocator* m_Allocator      = nullptr;
        nri::DescriptorPool*     m_DescriptorPool = nullptr;
    };

    AME_RHI_SCOPED_RESOURCE(DescriptorTable);
} // namespace Ame::Rhi