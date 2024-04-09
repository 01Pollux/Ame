#pragma once

#include <Core/Subsystem.hpp>

#include <Core/Coroutine.hpp>

namespace Ame
{
    struct CoroutineSubsystem : ExternSharedSubsystem<Co::runtime>, kgr::final
    {
    };

    auto service_map(const Ptr<Co::runtime>&) -> CoroutineSubsystem;
} // namespace Ame
