#pragma once

#include <Core/Subsystem.hpp>

#include <Ecs/Universe.hpp>

namespace Ame::Ecs
{
    struct EntitySubsystem : SingleSubsystem<Universe>, kgr::final
    {
    };

    auto service_map(const Universe&) -> EntitySubsystem;
} // namespace Ame::Ecs
