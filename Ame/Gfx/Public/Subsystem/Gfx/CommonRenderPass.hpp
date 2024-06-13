#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Rhi/Device.hpp>

#include <Gfx/Cache/CommonRenderPass.hpp>

namespace Ame::Gfx::Cache
{
    struct CommonRenderPassSubsystem : SingleSubsystem<
                                           CommonRenderPass,
                                           Dependency<
                                               Rhi::DeviceSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const CommonRenderPass&) -> CommonRenderPassSubsystem;
} // namespace Ame::Gfx::Cache
