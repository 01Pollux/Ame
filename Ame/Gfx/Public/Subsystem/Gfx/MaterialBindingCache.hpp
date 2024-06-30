#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/EngineFrame.hpp>
#include <Subsystem/Rhi/Device.hpp>

#include <Gfx/Cache/MaterialBindingCache.hpp>

namespace Ame::Gfx::Cache
{
    struct MaterialBindingCacheSubsystem : SingleSubsystem<
                                               MaterialBindingCache,
                                               Dependency<
                                                   EngineFrameSubsystem,
                                                   Rhi::DeviceSubsystem>>,
                                           kgr::final
    {
    };

    auto service_map(const MaterialBindingCache&) -> MaterialBindingCacheSubsystem;
} // namespace Ame::Gfx::Cache
