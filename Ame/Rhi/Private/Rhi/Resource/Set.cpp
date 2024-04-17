#include <Rhi/Resource/Set.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

namespace Ame::Rhi
{
    void DescriptorSet::SetRanges(
        Device&                                    RhiDevice,
        uint32_t                                   BaseRange,
        std::span<const DescriptorRangeUpdateDesc> RangeUpdateDescs)
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDescriptorRanges(*Unwrap(), BaseRange, Count32(RangeUpdateDescs), RangeUpdateDescs.data());
    }

    void DescriptorSet::SetRange(
        Device&                          RhiDevice,
        uint32_t                         BaseRange,
        const DescriptorRangeUpdateDesc& RangeUpdateDesc)
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDescriptorRanges(*Unwrap(), BaseRange, 1, &RangeUpdateDesc);
    }

    void DescriptorSet::SetDynamicBuffers(
        Device&                           RhiDevice,
        uint32_t                          BaseBuffer,
        std::span<const nri::Descriptor*> Buffers)
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDynamicConstantBuffers(*Unwrap(), BaseBuffer, Count32(Buffers), Buffers.data());
    }

    void DescriptorSet::SetDynamicBuffer(
        Device&                RhiDevice,
        uint32_t               BaseBuffer,
        const nri::Descriptor* Buffer)
    {

        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.UpdateDynamicConstantBuffers(*Unwrap(), BaseBuffer, 1, &Buffer);
    }

    void DescriptorSet::CopyTo(
        Device&                      RhiDevice,
        const DescriptorSetCopyDesc& CopyDesc)
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.CopyDescriptorSet(*Unwrap(), CopyDesc);
    }

    //

    nri::DescriptorSet* DescriptorSet::Unwrap() const
    {
        return m_Set;
    }
} // namespace Ame::Rhi