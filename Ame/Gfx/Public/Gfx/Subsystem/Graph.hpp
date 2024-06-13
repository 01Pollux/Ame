#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/Subsystem/Frame.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <Ecs/Subsystem/Universe.hpp>

#include <RG/Graph.hpp>

namespace Ame::Gfx::RG
{
    struct GraphSubsystem : SingleSubsystem<
                                Graph,
                                Dependency<FrameTimerSubsystem,
                                           Rhi::DeviceSubsystem,
                                           Ecs::UniverseSubsystem>>,
                            kgr::final
    {
    };

    auto service_map(const Graph&) -> GraphSubsystem;
} // namespace Ame::Gfx::RG
