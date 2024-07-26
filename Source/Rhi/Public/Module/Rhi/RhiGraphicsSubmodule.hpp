#pragma once

#include <Module/Submodule.hpp>
#include <Rhi/Device/RhiDevice.hpp>
#include <Module/Rhi/Config.hpp>

namespace Ame
{
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