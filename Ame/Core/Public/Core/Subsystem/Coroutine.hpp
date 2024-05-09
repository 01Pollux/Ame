#pragma once

#include <Core/Subsystem.hpp>

#include <Core/Coroutine.hpp>

namespace Ame
{
    struct CoroutineSubsystem : SingleSubsystem<
                                    Co::runtime>,
                                kgr::final
    {
    };

    auto service_map(const Co::runtime&) -> CoroutineSubsystem;
} // namespace Ame
