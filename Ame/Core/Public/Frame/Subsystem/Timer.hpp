#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/FrameTimer.hpp>

namespace Ame
{
    struct FrameTimerSubsystem : SingleSubsystem<
                                     FrameTimer>,
                                 kgr::final
    {
    };

    auto service_map(const FrameTimer&) -> FrameTimerSubsystem;
} // namespace Ame
