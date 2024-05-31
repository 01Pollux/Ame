#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>

#include <Rhi/Staging/DeferredStagingManager.hpp>

namespace Ame::Rhi::Staging
{
    struct DeferredStagingManagerSubsystem : SingleSubsystem<
                                                 DeferredStagingManager,
                                                 Dependency<
                                                     DeviceSubsystem>>,
                                             kgr::final
    {
    };

    auto service_map(const DeferredStagingManager&) -> DeferredStagingManagerSubsystem;
} // namespace Ame::Rhi::Staging
