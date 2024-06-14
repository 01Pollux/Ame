#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/Frame.hpp>
#include <Subsystem/Core/Timer.hpp>
#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Rhi/DeferredStagingStorage.hpp>
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
                                       Rhi::Staging::DeferredStagingManagerSubsystem,
                                       Ecs::UniverseSubsystem,
                                       RG::GraphSubsystem,
                                       EntityCompositorSubsystem,
                                       Cache::CommonRenderPassSubsystem>>,
                               kgr::final
    {
    };

    auto service_map(const Renderer&) -> RendererSubsystem;
} // namespace Ame::Gfx
