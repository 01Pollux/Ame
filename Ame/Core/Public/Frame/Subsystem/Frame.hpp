#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/SyncFrame.hpp>
#include <Frame/AsyncFrame.hpp>

#include <Frame/Subsystem/Timer.hpp>

namespace Ame
{
    struct SyncFrameSubsystem;
    struct AsyncFrameSubsystem;

    struct FrameSubsystem : AbstractSubsystem<IFrame>, kgr::defaults_to<SyncFrameSubsystem>
    {
    };

    struct SyncFrameSubsystem : AutoWiredSingleSubsystem<SyncFrame>, kgr::overrides<FrameSubsystem>, kgr::final
    {
    };

    struct AsyncFrameSubsystem : AutoWiredSingleSubsystem<AsyncFrame>, kgr::overrides<FrameSubsystem>, kgr::final
    {
    };

    auto service_map(IFrame&) -> FrameSubsystem;
} // namespace Ame
