#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Ecs/Subsystem/Universe.hpp>
#include <Gfx/Subsystem/Renderer.hpp>
#include <Gfx/Subsystem/CommonPipelineState.hpp>
#include <Gfx/Subsystem/ShaderCache.hpp>
#include <Gfx/Subsystem/MaterialBindingCache.hpp>

#include <FlappyRocket/Game.hpp>

namespace Ame::FlappyRocket
{
    struct FlappyRocketGameSubsystem : UniqueSubsystem<
                                           FlappyRocketGame,
                                           Dependency<
                                               Rhi::DeviceSubsystem,
                                               Ecs::UniverseSubsystem,
                                               Gfx::RendererSubsystem,
                                               Gfx::Cache::CommonPipelineStateSubsystem,
                                               Gfx::Cache::ShaderCacheSubsystem,
                                               Gfx::Cache::MaterialBindingCacheSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const FlappyRocketGame&) -> FlappyRocketGameSubsystem;
} // namespace Ame::FlappyRocket
