#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/EngineFrame.hpp>
#include <Subsystem/Core/FrameTimer.hpp>

#include <Ecs/Universe.hpp>

namespace Ame::Ecs
{
    struct UniverseSubsystem : SingleSubsystem<
                                   Universe,
                                   Dependency<
                                       EngineFrameSubsystem,
                                       FrameTimerSubsystem>>,
                               kgr::final
    {
    };

    auto service_map(const Universe&) -> UniverseSubsystem;
} // namespace Ame::Ecs
