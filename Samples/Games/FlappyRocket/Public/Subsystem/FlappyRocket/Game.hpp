#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Ecs/Universe.hpp>
#include <Subsystem/Gfx/ShaderCache.hpp>
#include <Subsystem/Asset/Storage.hpp>
#include <Subsystem/Gfx/MaterialCompiler.hpp>

#include <FlappyRocket/Game.hpp>

namespace Ame::FlappyRocket
{
    struct FlappyRocketGameSubsystem : UniqueSubsystem<
                                           FlappyRocketGame,
                                           Dependency<
                                               Ecs::UniverseSubsystem,
                                               Asset::StorageSubsystem,
                                               Gfx::Cache::ShaderCacheSubsystem,
                                               Gfx::Shading::MaterialCompilerSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const FlappyRocketGame&) -> FlappyRocketGameSubsystem;
} // namespace Ame::FlappyRocket
