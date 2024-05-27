#pragma once

#include <Core/Subsystem.hpp>

#include <Gfx/Cache/CommonRenderPass.hpp>

namespace Ame::Gfx::Cache
{
    struct CommonRenderPassSubsystem : SingleSubsystem<
                                           CommonRenderPass,
                                           Dependency<>>,
                                       kgr::final
    {
    };

    auto service_map(const CommonRenderPass&) -> CommonRenderPassSubsystem;
} // namespace Ame::Gfx::Cache
