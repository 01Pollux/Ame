#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/Subsystem/Timer.hpp>
#include <Frame/Subsystem/Frame.hpp>

#include <Ecs/Universe.hpp>

namespace Ame::Ecs
{
    struct UniverseSubsystem : SingleSubsystem<
                                   Universe,
                                   Dependency<EngineFrameSubsystem,
                                              FrameTimerSubsystem>>,
                               kgr::final
    {
    };

    auto service_map(const Universe&) -> UniverseSubsystem;
} // namespace Ame::Ecs
