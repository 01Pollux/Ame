#pragma once

#include <Module/Submodule.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame
{
    // {116F0CD7-9584-417E-BA5E-5EC61E74C7E6}
    static const UId IID_RhiGraphicsSubmodule = { 0x116f0cd7, 0x9584, 0x417e, { 0xba, 0x5e, 0x5e, 0xc6, 0x1e, 0x74, 0xc7, 0xe6 } };

    class RhiGraphicsSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_RhiGraphicsSubmodule, IID_BaseSubmodule, Base,
            m_RhiDevice);

    public:
        RhiGraphicsSubmodule(
            IReferenceCounters*  counters,
            Ptr<Rhi::IRhiDevice> rhiDevice) :
            Base(counters, IID_RhiGraphicsSubmodule),
            m_RhiDevice(std::move(rhiDevice))
        {
        }

    private:
        Ptr<Rhi::IRhiDevice> m_RhiDevice;
    };
} // namespace Ame