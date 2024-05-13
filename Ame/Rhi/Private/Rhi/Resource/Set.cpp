#include <Rhi/Resource/Set.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

namespace Ame::Rhi
{
    void DescriptorSet::SetRanges(
        uint32_t                                   BaseRange,
        std::span<const DescriptorRangeUpdateDesc> RangeUpdateDescs)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDescriptorRanges(*Unwrap(), BaseRange, Count32(RangeUpdateDescs), RangeUpdateDescs.data());
    }

    void DescriptorSet::SetRange(
        uint32_t                         BaseRange,
        const DescriptorRangeUpdateDesc& RangeUpdateDesc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDescriptorRanges(*Unwrap(), BaseRange, 1, &RangeUpdateDesc);
    }

    void DescriptorSet::SetDynamicBuffers(
        uint32_t                          BufferOffset,
        std::span<const nri::Descriptor*> Buffers)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDynamicConstantBuffers(*Unwrap(), BufferOffset, Count32(Buffers), Buffers.data());
    }

    void DescriptorSet::SetDynamicBuffer(
        uint32_t               BufferOffset,
        const nri::Descriptor* Buffer)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDynamicConstantBuffers(*Unwrap(), BufferOffset, 1, &Buffer);
    }

    void DescriptorSet::CopyTo(
        const DescriptorSetCopyDesc& CopyDesc)
    {
        auto& Nri     = m_RhiDevice->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CopyDescriptorSet(*Unwrap(), CopyDesc);
    }

    //

    nri::DescriptorSet* DescriptorSet::Unwrap() const
    {
        return m_Set;
    }
} // namespace Ame::Rhi