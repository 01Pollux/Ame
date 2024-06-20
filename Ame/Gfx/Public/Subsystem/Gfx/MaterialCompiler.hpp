#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Gfx/CommonPipelineLayout.hpp>
#include <Subsystem/Gfx/ShaderCache.hpp>
#include <Subsystem/Gfx/CommonPipelineState.hpp>

#include <Gfx/Shading/MaterialCompiler.hpp>

namespace Ame::Gfx::Shading
{
    struct MaterialCompilerSubsystem : SingleSubsystem<
                                           MaterialCompiler,
                                           Dependency<
                                               Rhi::DeviceSubsystem,
                                               Cache::CommonPipelineLayoutSubsystem,
                                               Cache::ShaderCacheSubsystem,
                                               Cache::CommonPipelineStateSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const MaterialCompiler&) -> MaterialCompilerSubsystem;
} // namespace Ame::Gfx::Shading
