#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Core/Coroutine.hpp>
#include <Subsystem/Asset/Storage.hpp>

#include <Gfx/Cache/ShaderCache.hpp>

namespace Ame::Gfx::Cache
{
    struct ShaderCacheSubsystem : SingleSubsystem<
                                      ShaderCache,
                                      Dependency<
                                          CoroutineSubsystem,
                                          Rhi::DeviceSubsystem,
                                          Asset::StorageSubsystem>>,
                                  kgr::final
    {
    };

    auto service_map(const ShaderCache&) -> ShaderCacheSubsystem;
} // namespace Ame::Gfx::Cache
