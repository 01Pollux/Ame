#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Core/Coroutine.hpp>
#include <Subsystem/Gfx/CommonPipelineLayout.hpp>
#include <Subsystem/Gfx/CommonShader.hpp>

#include <Gfx/Cache/CommonPipelineState.hpp>

namespace Ame::Gfx::Cache
{
    struct CommonPipelineStateSubsystem : SingleSubsystem<
                                              CommonPipelineState,
                                              Dependency<
                                                  CoroutineSubsystem,
                                                  Rhi::DeviceSubsystem,
                                                  CommonPipelineLayoutSubsystem,
                                                  CommonShaderSubsystem>>,
                                          kgr::final
    {
    };

    auto service_map(const CommonPipelineState&) -> CommonPipelineStateSubsystem;
} // namespace Ame::Gfx::Cache
