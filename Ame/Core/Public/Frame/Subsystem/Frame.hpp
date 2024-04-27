#pragma once

#include <Core/Subsystem.hpp>

#include <Frame/SyncFrame.hpp>
#include <Frame/AsyncFrame.hpp>

#include <Frame/Subsystem/Timer.hpp>

namespace Ame
{
    struct SyncFrameSubsystem;
    struct AsyncFrameSubsystem;

    struct FrameSubsystem : AbstractSharedSubsystem<IFrame>, kgr::defaults_to<SyncFrameSubsystem>
    {
    };

    struct SyncFrameSubsystem : AutoWiredSharedSubsystem<SyncFrame>, kgr::overrides<FrameSubsystem>, kgr::final
    {
    };

    struct AsyncFrameSubsystem : AutoWiredSharedSubsystem<AsyncFrame>, kgr::overrides<FrameSubsystem>, kgr::final
    {
    };

    auto service_map(const Ptr<IFrame>&) -> FrameSubsystem;
} // namespace Ame
