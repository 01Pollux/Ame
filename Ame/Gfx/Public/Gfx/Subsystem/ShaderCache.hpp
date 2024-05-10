#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Core/Subsystem/Coroutine.hpp>

#include <Gfx/Cache/ShaderCache.hpp>

namespace Ame::Gfx::Cache
{
    struct ShaderCacheSubsystem : SingleSubsystem<
                                      ShaderCache,
                                      Dependency<
                                          Rhi::DeviceSubsystem,
                                          CoroutineSubsystem>>,
                                  kgr::final
    {
    };

    auto service_map(const ShaderCache&) -> ShaderCacheSubsystem;
} // namespace Ame::Gfx::Cache
