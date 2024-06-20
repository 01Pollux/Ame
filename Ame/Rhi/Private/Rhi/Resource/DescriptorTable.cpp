#include <Rhi/Resource/DescriptorTable.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    DescriptorTable::DescriptorTable(
        DeviceResourceAllocator& allocator,
        nri::DescriptorPool*     descriptorPool) :
        m_Allocator(&allocator),
        m_DescriptorPool(descriptorPool)
    {
    }

    void DescriptorTable::Release(
        bool defer)
    {
        if (m_DescriptorPool)
        {
            if (defer)
            {
                auto& frameManager = m_Allocator->GetFrameManager();
                auto& frame        = frameManager.GetCurrentFrame();

                auto lock = m_Allocator->TryLock();
                frame.DeferRelease(*m_DescriptorPool);
            }
            else
            {
                auto& nriCore = m_Allocator->GetNriCore();
                nriCore.DestroyDescriptorPool(*m_DescriptorPool);
            }
            m_DescriptorPool = nullptr;
        }
    }

    //

    void DescriptorTable::SetName(
        const char* name) const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.SetDescriptorPoolDebugName(*m_DescriptorPool, name);
    }

    nri::DescriptorPool* const& DescriptorTable::Unwrap() const
    {
        return m_DescriptorPool;
    }

    //

    void DescriptorTable::Reset()
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.ResetDescriptorPool(*m_DescriptorPool);
    }

    DescriptorSet DescriptorTable::AllocateSet(
        const nri::PipelineLayout& layout,
        uint32_t                   layoutSlot,
        uint32_t                   variableCount)
    {
        nri::DescriptorSet* nriSet = nullptr;

        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.AllocateDescriptorSets(*m_DescriptorPool, layout, layoutSlot, &nriSet, 1, variableCount);

        return DescriptorSet(*m_Allocator, nriSet);
    }

    std::vector<DescriptorSet> DescriptorTable::AllocateSets(
        const nri::PipelineLayout& layout,
        uint32_t                   layoutSlot,
        uint32_t                   instanceCount,
        uint32_t                   variableCount)
    {
        std::vector<nri::DescriptorSet*> nriSets(instanceCount);

        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.AllocateDescriptorSets(*m_DescriptorPool, layout, layoutSlot, nriSets.data(), instanceCount, variableCount);

        return nriSets |
               std::views::transform([this](nri::DescriptorSet* nriSet)
                                     { return DescriptorSet(*m_Allocator, nriSet); }) |
               std::ranges::to<std::vector>();
    }
} // namespace Ame::Rhi