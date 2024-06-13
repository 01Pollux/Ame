#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Core/Coroutine.hpp>

#include <Gfx/Cache/CommonPipelineLayout.hpp>

namespace Ame::Gfx::Cache
{
    struct CommonPipelineLayoutSubsystem : SingleSubsystem<
                                               CommonPipelineLayout,
                                               Dependency<
                                                   CoroutineSubsystem,
                                                   Rhi::DeviceSubsystem>>,
                                           kgr::final
    {
    };

    auto service_map(const CommonPipelineLayout&) -> CommonPipelineLayoutSubsystem;
} // namespace Ame::Gfx::Cache
