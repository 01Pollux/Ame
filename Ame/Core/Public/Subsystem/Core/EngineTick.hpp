#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/EngineTick.hpp>

namespace Ame
{
    struct EngineTickSubsystem : SingleSubsystem<
                                     IEngineTick>,
                                 kgr::abstract
    {
    };

    auto service_map(const IEngineTick&) -> EngineTickSubsystem;
} // namespace Ame
