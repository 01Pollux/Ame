#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/EngineFrame.hpp>
#include <Subsystem/Core/FrameTimer.hpp>
#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Ecs/Universe.hpp>
#include <Subsystem/RG/RenderGraph.hpp>
#include <Subsystem/Gfx/EntityCompositor.hpp>
#include <Subsystem/Gfx/CommonRenderPass.hpp>

#include <Gfx/Renderer.hpp>

namespace Ame::Gfx
{
    struct RendererSubsystem : SingleSubsystem<
                                   Renderer,
                                   Dependency<
                                       EngineFrameSubsystem,
                                       FrameTimerSubsystem,
                                       Rhi::DeviceSubsystem,
                                       Ecs::UniverseSubsystem,
                                       RG::GraphSubsystem,
                                       EntityCompositorSubsystem,
                                       Cache::CommonRenderPassSubsystem>>,
                               kgr::overrides<EngineTickSubsystem>,
                               kgr::final
    {
    };

    auto service_map(const Renderer&) -> RendererSubsystem;
} // namespace Ame::Gfx
