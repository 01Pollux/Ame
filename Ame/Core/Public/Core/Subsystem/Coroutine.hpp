#pragma once

#include <Core/Subsystem.hpp>

#include <Core/Coroutine.hpp>

namespace Ame
{
    struct CoroutineSubsystem : public ExternSharedSubsystem<Co::runtime>
    {
    };

    auto service_map(const Ptr<Co::runtime>&) -> CoroutineSubsystem;
} // namespace Ame
