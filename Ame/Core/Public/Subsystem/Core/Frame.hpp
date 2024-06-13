#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/Timer.hpp>

#include <Frame/EngineFrame.hpp>

namespace Ame
{
    struct EngineFrameSubsystem : SingleSubsystem<
                                      EngineFrame,
                                      Dependency<
                                          FrameTimerSubsystem>>,
                                  kgr::final
    {
    };

    auto service_map(const EngineFrame&) -> EngineFrameSubsystem;
} // namespace Ame
