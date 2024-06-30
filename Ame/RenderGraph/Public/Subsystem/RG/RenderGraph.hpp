#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/EngineFrame.hpp>
#include <Subsystem/Rhi/Device.hpp>

#include <RG/Graph.hpp>

namespace Ame::RG
{
    struct GraphSubsystem : SingleSubsystem<
                                Graph,
                                Dependency<
                                    FrameTimerSubsystem,
                                    Rhi::DeviceSubsystem>>,
                            kgr::final
    {
    };

    auto service_map(const Graph&) -> GraphSubsystem;
} // namespace Ame::RG
