#pragma once

#include <Core/Subsystem.hpp>

#include <Ecs/Subsystem/Universe.hpp>
#include <Gfx/Subsystem/Renderer.hpp>
#include <Gfx/Subsystem/PipelineLayoutCache.hpp>

#include <FlappyRocket/Game.hpp>

namespace Ame::FlappyRocket
{
    struct FlappyRocketGameSubsystem : UniqueSubsystem<
                                           FlappyRocketGame,
                                           Dependency<Ecs::UniverseSubsystem,
                                                      Gfx::RendererSubsystem,
                                                      Gfx::Cache::PipelineLayoutCacheSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const FlappyRocketGame&) -> FlappyRocketGameSubsystem;
} // namespace Ame::FlappyRocket
