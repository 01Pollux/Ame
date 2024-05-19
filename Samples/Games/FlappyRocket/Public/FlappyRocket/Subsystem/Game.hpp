#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Ecs/Subsystem/Universe.hpp>
#include <Gfx/Subsystem/Renderer.hpp>
#include <Gfx/Subsystem/PipelineStateCache.hpp>

#include <FlappyRocket/Game.hpp>

namespace Ame::FlappyRocket
{
    struct FlappyRocketGameSubsystem : UniqueSubsystem<
                                           FlappyRocketGame,
                                           Dependency<
                                               Rhi::DeviceSubsystem,
                                               Ecs::UniverseSubsystem,
                                               Gfx::RendererSubsystem,
                                               Gfx::Cache::PipelineStateCacheSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const FlappyRocketGame&) -> FlappyRocketGameSubsystem;
} // namespace Ame::FlappyRocket
