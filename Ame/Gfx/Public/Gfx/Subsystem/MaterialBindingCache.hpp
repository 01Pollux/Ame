#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/Subsystem/Frame.hpp>
#include <Rhi/Subsystem/Device.hpp>

#include <Gfx/Cache/MaterialBindingCache.hpp>

namespace Ame::Gfx::Cache
{
    struct MaterialBindingCacheSubsystem : SingleSubsystem<
                                               MaterialBindingCache,
                                               Dependency<
                                                   Rhi::DeviceSubsystem,
                                                   EngineFrameSubsystem>>,
                                           kgr::final
    {
    };

    auto service_map(const MaterialBindingCache&) -> MaterialBindingCacheSubsystem;
} // namespace Ame::Gfx::Cache
