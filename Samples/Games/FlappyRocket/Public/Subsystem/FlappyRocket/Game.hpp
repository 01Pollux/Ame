#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Ecs/Universe.hpp>
#include <Subsystem/Gfx/ShaderCache.hpp>
#include <Subsystem/Asset/Storage.hpp>

#include <FlappyRocket/Game.hpp>

namespace Ame::FlappyRocket
{
    struct FlappyRocketGameSubsystem : UniqueSubsystem<
                                           FlappyRocketGame,
                                           Dependency<
                                               Rhi::DeviceSubsystem,
                                               Ecs::UniverseSubsystem,
                                               Asset::StorageSubsystem,
                                               Gfx::Cache::ShaderCacheSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const FlappyRocketGame&) -> FlappyRocketGameSubsystem;
} // namespace Ame::FlappyRocket
