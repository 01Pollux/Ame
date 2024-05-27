#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/Subsystem/Frame.hpp>
#include <Frame/Subsystem/Timer.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <Ecs/Subsystem/Universe.hpp>
#include <Gfx/Subsystem/Graph.hpp>
#include <Gfx/Subsystem/CommonRenderPass.hpp>

#include <Gfx/Renderer.hpp>

namespace Ame::Gfx
{
    struct RendererSubsystem : SingleSubsystem<
                                   Renderer,
                                   Dependency<EngineFrameSubsystem,
                                              FrameTimerSubsystem,
                                              Rhi::DeviceSubsystem,
                                              Ecs::UniverseSubsystem,
                                              RG::GraphSubsystem,
                                              Cache::CommonRenderPassSubsystem>>,
                               kgr::final
    {
    };

    auto service_map(const Renderer&) -> RendererSubsystem;
} // namespace Ame::Gfx
