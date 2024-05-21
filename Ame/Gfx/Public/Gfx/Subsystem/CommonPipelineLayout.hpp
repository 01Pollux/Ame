#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Core/Subsystem/Coroutine.hpp>

#include <Gfx/Cache/CommonPipelineLayout.hpp>

namespace Ame::Gfx::Cache
{
    struct CommonPipelineLayoutSubsystem : SingleSubsystem<
                                               CommonPipelineLayout,
                                               Dependency<
                                                   Rhi::DeviceSubsystem,
                                                   CoroutineSubsystem>>,
                                           kgr::final
    {
    };

    auto service_map(const CommonPipelineLayout&) -> CommonPipelineLayoutSubsystem;
} // namespace Ame::Gfx::Cache
