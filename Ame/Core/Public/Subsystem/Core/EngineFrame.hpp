#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/Coroutine.hpp>
#include <Subsystem/Core/EngineTick.hpp>

#include <Frame/EngineFrame.hpp>

namespace Ame
{
    struct EngineFrameSubsystem : SingleSubsystem<
                                      EngineFrame,
                                      Dependency<
                                          CoroutineSubsystem>>,
                                  kgr::overrides<EngineTickSubsystem>,
                                  kgr::final
    {
    };

    auto service_map(const EngineFrame&) -> EngineFrameSubsystem;
} // namespace Ame
