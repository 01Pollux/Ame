#include <Rhi/Device/DescriptorAllocator.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void DescriptorAllocator::Initialize(
        DeviceImpl&                     RhiDevice,
        const DescriptorAllocationDesc& Desc)
    {
        m_RhiDevice = &RhiDevice;

        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        nri::DescriptorPoolDesc NriDesc{
            .descriptorSetMaxNum           = Desc.DescriptorSetMaxCount,
            .samplerMaxNum                 = Desc.SamplerMaxCount,
            .constantBufferMaxNum          = Desc.ConstantBufferMaxCount,
            .dynamicConstantBufferMaxNum   = Desc.DynamicConstantBufferMaxCount,
            .textureMaxNum                 = Desc.TextureMaxCount,
            .storageTextureMaxNum          = Desc.StorageTextureMaxCount,
            .bufferMaxNum                  = Desc.BufferMaxCount,
            .storageBufferMaxNum           = Desc.StorageBufferMaxCount,
            .structuredBufferMaxNum        = Desc.StructuredBufferMaxCount,
            .storageStructuredBufferMaxNum = Desc.StorageStructuredBufferMaxCount,
            .accelerationStructureMaxNum   = Desc.AccelerationStructureMaxCount,
        };
        ThrowIfFailed(NriCore.CreateDescriptorPool(m_RhiDevice->GetDevice(), NriDesc, m_Pool), "Failed to create descriptor pool");
    }

    void DescriptorAllocator::Shutdown()
    {
        Log::Rhi().Assert(m_Pool != nullptr, "Tried to shutdown descriptor allocator which was not initialized.");

        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.DestroyDescriptorPool(*m_Pool);
        m_Pool = nullptr;
    }

    //

    void DescriptorAllocator::ResetPool()
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.ResetDescriptorPool(*m_Pool);
    }

    nri::DescriptorPool* DescriptorAllocator::GetPool() noexcept
    {
        return m_Pool;
    }

    std::vector<DescriptorSet> DescriptorAllocator::Allocate(
        const nri::PipelineLayout& Layout,
        uint32_t                   LayoutSlot,
        uint32_t                   InstanceCount,
        uint32_t                   VariableCount)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        if (InstanceCount == 1)
        {
            nri::DescriptorSet* Set = nullptr;
            NriCore.AllocateDescriptorSets(*m_Pool, Layout, LayoutSlot, &Set, 1, VariableCount);
            return { DescriptorSet{ Set } };
        }
        else
        {
            std::vector<nri::DescriptorSet*> NriDescriptorSets(InstanceCount);
            NriCore.AllocateDescriptorSets(*m_Pool, Layout, LayoutSlot, NriDescriptorSets.data(), InstanceCount, VariableCount);

            return NriDescriptorSets |
                   std::views::transform([](nri::DescriptorSet* Set)
                                         { return DescriptorSet{ Set }; }) |
                   std::ranges::to<std::vector>();
        }
    }
} // namespace Ame::Rhi