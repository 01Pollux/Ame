#include <Rhi/Resource/Set.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

namespace Ame::Rhi
{
    void DescriptorSet::SetRanges(
        uint32_t                                   baseRange,
        std::span<const DescriptorRangeUpdateDesc> rangeUpdateDescs)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.UpdateDescriptorRanges(*Unwrap(), baseRange, Count32(rangeUpdateDescs), rangeUpdateDescs.data());
    }

    void DescriptorSet::SetRange(
        uint32_t                         baseRange,
        const DescriptorRangeUpdateDesc& rangeUpdateDescs)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.UpdateDescriptorRanges(*Unwrap(), baseRange, 1, &rangeUpdateDescs);
    }

    void DescriptorSet::SetDynamicBuffers(
        uint32_t                          baseBufferInSet,
        std::span<const nri::Descriptor*> buffers)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.UpdateDynamicConstantBuffers(*Unwrap(), baseBufferInSet, Count32(buffers), buffers.data());
    }

    void DescriptorSet::SetDynamicBuffer(
        uint32_t               baseBufferInSet,
        const nri::Descriptor* buffer)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.UpdateDynamicConstantBuffers(*Unwrap(), baseBufferInSet, 1, &buffer);
    }

    void DescriptorSet::CopyTo(
        const DescriptorSetCopyDesc& copyDesc)
    {
        auto& nriUtils = m_RhiDevice->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.CopyDescriptorSet(*Unwrap(), copyDesc);
    }

    //

    nri::DescriptorSet* DescriptorSet::Unwrap() const
    {
        return m_Set;
    }
} // namespace Ame::Rhi