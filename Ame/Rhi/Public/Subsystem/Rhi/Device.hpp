#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/Frame.hpp>
#include <Subsystem/Core/Coroutine.hpp>

#include <Rhi/Device/Device.hpp>

namespace Ame::Rhi
{
    struct DeviceSubsystem : SingleSubsystem<
                                 Device,
                                 Dependency<
                                     EngineFrameSubsystem,
                                     CoroutineSubsystem>>,
                             kgr::supplied,
                             kgr::final
    {
    };

    auto service_map(const Device&) -> DeviceSubsystem;
} // namespace Ame::Rhi
