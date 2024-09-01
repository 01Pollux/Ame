#pragma once

#include <Core/Interface.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Interfaces
{
    // {116F0CD7-9584-417E-BA5E-5EC61E74C7E6}
    inline constexpr UId IID_RhiBackend{
        0x116f0cd7, 0x9584, 0x417e, { 0xba, 0x5e, 0x5e, 0xc6, 0x1e, 0x74, 0xc7, 0xe6 }
    };

    class RhiBackend final : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(IID_RhiBackend, Base, m_RhiDevice);

    private:
        IMPLEMENT_INTERFACE_CTOR(RhiBackend, Ptr<Rhi::IRhiDevice> rhiDevice) :
            Base(counters), m_RhiDevice(std::move(rhiDevice))
        {
        }

    public:
        [[nodiscard]] auto& GetRhiDevice() const noexcept
        {
            return m_RhiDevice;
        }

    private:
        Ptr<Rhi::IRhiDevice> m_RhiDevice;
    };
} // namespace Ame::Interfaces