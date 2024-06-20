#include <Rhi/Device/DescriptorAllocator.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void DescriptorAllocator::Initialize(
        DeviceImpl&                     rhiDevice,
        const DescriptorAllocationDesc& desc)
    {
        m_RhiDevice = &rhiDevice;

        auto& nriUtils   = m_RhiDevice->GetNRI();
        auto& nriCore    = *nriUtils.GetCoreInterface();
        auto& deviceDesc = nriCore.GetDeviceDesc(m_RhiDevice->GetDevice());

        nri::DescriptorPoolDesc descriptorPoolDesc{
            .descriptorSetMaxNum           = desc.DescriptorSetMaxCount,
            .samplerMaxNum                 = desc.SamplerMaxCount,
            .constantBufferMaxNum          = desc.ConstantBufferMaxCount,
            .dynamicConstantBufferMaxNum   = desc.DynamicConstantBufferMaxCount,
            .textureMaxNum                 = desc.TextureMaxCount,
            .storageTextureMaxNum          = desc.StorageTextureMaxCount,
            .bufferMaxNum                  = desc.BufferMaxCount,
            .storageBufferMaxNum           = desc.StorageBufferMaxCount,
            .structuredBufferMaxNum        = desc.StructuredBufferMaxCount,
            .storageStructuredBufferMaxNum = desc.StorageStructuredBufferMaxCount,
            .accelerationStructureMaxNum   = deviceDesc.isRayTracingSupported ? desc.AccelerationStructureMaxCount : 0
        };
        ThrowIfFailed(nriCore.CreateDescriptorPool(m_RhiDevice->GetDevice(), descriptorPoolDesc, m_Pool), "Failed to create descriptor pool");
    }

    void DescriptorAllocator::Shutdown()
    {
        Log::Rhi().Assert(m_Pool != nullptr, "Tried to shutdown descriptor allocator which was not initialized.");

        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.DestroyDescriptorPool(*m_Pool);
        m_Pool = nullptr;
    }

    //

    void DescriptorAllocator::ResetPool()
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.ResetDescriptorPool(*m_Pool);
    }

    nri::DescriptorPool* DescriptorAllocator::GetPool() noexcept
    {
        return m_Pool;
    }

    DescriptorSet DescriptorAllocator::Allocate(
        const nri::PipelineLayout& layout,
        uint32_t                   layoutSlot,
        uint32_t                   variableCount)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nri::DescriptorSet* nriSet = nullptr;
        nriCore.AllocateDescriptorSets(*m_Pool, layout, layoutSlot, &nriSet, 1, variableCount);
        return { DescriptorSet{ m_RhiDevice, nriSet } };
    }

    std::vector<DescriptorSet> DescriptorAllocator::Allocate(
        const nri::PipelineLayout& layout,
        uint32_t                   layoutSlot,
        uint32_t                   instanceCount,
        uint32_t                   variableCount)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        std::vector<nri::DescriptorSet*> nriSets(instanceCount);
        nriCore.AllocateDescriptorSets(*m_Pool, layout, layoutSlot, nriSets.data(), instanceCount, variableCount);

        return nriSets |
               std::views::transform([this](nri::DescriptorSet* nriSet)
                                     { return DescriptorSet{ m_RhiDevice, nriSet }; }) |
               std::ranges::to<std::vector>();
    }
} // namespace Ame::Rhi