#pragma once

#include <Core/Subsystem.hpp>

#include <Rhi/Subsystem/Device.hpp>
#include <Core/Subsystem/Coroutine.hpp>
#include <Gfx/Subsystem/CommonPipelineLayout.hpp>
#include <Gfx/Subsystem/CommonShader.hpp>

#include <Gfx/Cache/CommonPipelineState.hpp>

namespace Ame::Gfx::Cache
{
    struct CommonPipelineStateSubsystem : SingleSubsystem<
                                              CommonPipelineState,
                                              Dependency<
                                                  Rhi::DeviceSubsystem,
                                                  CoroutineSubsystem,
                                                  CommonPipelineLayoutSubsystem,
                                                  CommonShaderSubsystem>>,
                                          kgr::final
    {
    };

    auto service_map(const CommonPipelineState&) -> CommonPipelineStateSubsystem;
} // namespace Ame::Gfx::Cache
