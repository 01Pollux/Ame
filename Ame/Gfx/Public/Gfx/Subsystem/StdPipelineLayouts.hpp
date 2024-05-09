#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Core/Subsystem/Coroutine.hpp>

#include <Gfx/Cache/StdPipelineLayouts.hpp>

namespace Ame::Gfx::Cache
{
    struct PipelineLayoutCacheSubsystem : SingleSubsystem<
                                              PipelineLayoutCache,
                                              Dependency<
                                                  Rhi::DeviceSubsystem,
                                                  CoroutineSubsystem>>,
                                          kgr::final
    {
    };

    auto service_map(const PipelineLayoutCache&) -> PipelineLayoutCacheSubsystem;
} // namespace Ame::Gfx::Cache
