#pragma once

#include <set>

#include <Rhi/Device/DescriptorDesc.hpp>
#include <Allocator/Utils/Buddy.hpp>

#include <Rhi/Resource/Set.hpp>
#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    class DescriptorAllocator
    {
    public:
        /// <summary>
        /// Initialize the descriptor allocator.
        /// </summary>
        void Initialize(
            DeviceImpl&                     RhiDevice,
            const DescriptorAllocationDesc& Desc);

        /// <summary>
        /// Release all memory associated with the allocator.
        /// </summary>
        void Shutdown();

    public:
        /// <summary>
        /// Reset descriptor pool.
        /// </summary>
        void ResetPool();

        /// <summary>
        /// Get descriptor pool.
        /// </summary>
        [[nodiscard]] nri::DescriptorPool* GetPool() noexcept;

        /// <summary>
        /// Allocate descriptor set.
        /// </summary>
        [[nodiscard]] DescriptorSet Allocate(
            const nri::PipelineLayout& Layout,
            uint32_t                   LayoutSlot,
            uint32_t                   VariableCount);

        /// <summary>
        /// Allocate descriptor sets.
        /// </summary>
        [[nodiscard]] std::vector<DescriptorSet> Allocate(
            const nri::PipelineLayout& Layout,
            uint32_t                   LayoutSlot,
            uint32_t                   InstanceCount,
            uint32_t                   VariableCount);

    private:
        DeviceImpl*          m_RhiDevice = nullptr;
        nri::DescriptorPool* m_Pool      = nullptr;
    };
} // namespace Ame::Rhi