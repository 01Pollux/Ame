#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/Coroutine.hpp>

#include <Rhi/Device/Device.hpp>

namespace Ame::Rhi
{
    struct RhiDeviceSubsystem : SingleSubsystem<
                                    RhiDevice,
                                    Dependency<
                                        CoroutineSubsystem>>,
                                kgr::supplied,
                                kgr::final
    {
    };

    auto service_map(const RhiDevice&) -> RhiDeviceSubsystem;
} // namespace Ame::Rhi
