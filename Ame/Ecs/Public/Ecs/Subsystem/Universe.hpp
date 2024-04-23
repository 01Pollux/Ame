#pragma once

#include <Core/Subsystem.hpp>

#include <Ecs/Universe.hpp>

namespace Ame::Ecs
{
    struct UniverseSubsystem : AutoWiredSingleSubsystem<Universe>, kgr::final
    {
    };

    auto service_map(const Universe&) -> UniverseSubsystem;
} // namespace Ame::Ecs
