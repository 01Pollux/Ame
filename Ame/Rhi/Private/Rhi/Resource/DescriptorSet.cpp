#include <Rhi/Resource/DescriptorSet.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    DescriptorSet::DescriptorSet(
        DeviceResourceAllocator& allocator,
        nri::DescriptorSet*      set) :
        m_Allocator(&allocator),
        m_Set(set)
    {
    }

    void DescriptorSet::SetRanges(
        uint32_t                                   baseRange,
        std::span<const DescriptorRangeUpdateDesc> rangeUpdateDescs)
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.UpdateDescriptorRanges(*Unwrap(), baseRange, Count32(rangeUpdateDescs), rangeUpdateDescs.data());
    }

    void DescriptorSet::SetRange(
        uint32_t                         baseRange,
        const DescriptorRangeUpdateDesc& rangeUpdateDescs)
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.UpdateDescriptorRanges(*Unwrap(), baseRange, 1, &rangeUpdateDescs);
    }

    void DescriptorSet::SetDynamicBuffers(
        uint32_t                          baseBufferInSet,
        std::span<const nri::Descriptor*> buffers)
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.UpdateDynamicConstantBuffers(*Unwrap(), baseBufferInSet, Count32(buffers), buffers.data());
    }

    void DescriptorSet::SetDynamicBuffer(
        uint32_t               baseBufferInSet,
        const nri::Descriptor* buffer)
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.UpdateDynamicConstantBuffers(*Unwrap(), baseBufferInSet, 1, &buffer);
    }

    void DescriptorSet::CopyTo(
        const DescriptorSetCopyDesc& copyDesc)
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.CopyDescriptorSet(*Unwrap(), copyDesc);
    }

    //

    void DescriptorSet::SetName(
        const char* name) const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.SetDescriptorSetDebugName(*Unwrap(), name);
    }

    nri::DescriptorSet* const& DescriptorSet::Unwrap() const
    {
        return m_Set;
    }
} // namespace Ame::Rhi