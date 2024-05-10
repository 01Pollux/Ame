#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Core/Subsystem/Coroutine.hpp>
#include <Gfx/Subsystem/PipelineLayoutCache.hpp>
#include <Asset/Subsystem/Storage.hpp>

#include <Gfx/Cache/PipelineStateCache.hpp>

namespace Ame::Gfx::Cache
{
    struct PipelineStateCacheSubsystem : SingleSubsystem<
                                             PipelineStateCache,
                                             Dependency<
                                                 Rhi::DeviceSubsystem,
                                                 CoroutineSubsystem,
                                                 PipelineLayoutCacheSubsystem,
                                                 Asset::StorageSubsystem>>,
                                         kgr::final
    {
    };

    auto service_map(const PipelineStateCache&) -> PipelineStateCacheSubsystem;
} // namespace Ame::Gfx::Cache
