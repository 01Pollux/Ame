#pragma once

#include <Core/Subsystem.hpp>

#include <Engine/Timer.hpp>

namespace Ame
{
    struct TimerSubsystem : SingleSubsystem<EngineTimer>, kgr::final
    {
    };

    auto service_map(const EngineTimer&) -> TimerSubsystem;
} // namespace Ame
