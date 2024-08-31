#pragma once

#include <Rhi/Device/RhiDevice.hpp>
#include <Interfaces/Rhi/Config.hpp>

namespace Ame::Interfaces
{
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