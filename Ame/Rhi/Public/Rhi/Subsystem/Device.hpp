#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Device.hpp>

namespace Ame::Rhi
{
    struct DeviceSubsystem : SingleSubsystem<Device>, kgr::supplied, kgr::final
    {
    };

    auto service_map(const Device&) -> DeviceSubsystem;
} // namespace Ame::Rhi
