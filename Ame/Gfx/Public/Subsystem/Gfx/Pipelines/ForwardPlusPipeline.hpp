#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Ecs/Universe.hpp>
#include <Subsystem/Gfx/EntityCompositor.hpp>
#include <Subsystem/Gfx/FrameBufferCache.hpp>
#include <Subsystem/Gfx/CommonPipelineState.hpp>
#include <Subsystem/Gfx/CommonShader.hpp>
#include <Subsystem/Gfx/MaterialBindingCache.hpp>
#include <Subsystem/Gfx/Pipelines/BaseGraphPipeline.hpp>

#include <Gfx/RenderGraph/Pipelines/ForwardPlusPipeline.hpp>

namespace Ame::Gfx
{
    struct ForwardPlusPipelineSubsystem : SingleSubsystem<
                                              ForwardPlusPipeline,
                                              Dependency<
                                                  RG::GraphSubsystem,
                                                  Ecs::UniverseSubsystem,
                                                  EntityCompositorSubsystem,
                                                  Cache::FrameDynamicBufferCacheSubsystem<>,
                                                  Cache::CommonShaderSubsystem,
                                                  Cache::CommonPipelineStateSubsystem,
                                                  Cache::MaterialBindingCacheSubsystem>>,
                                          kgr::overrides<GraphRendererSubsystem>,
                                          kgr::final
    {
    };

    auto service_map(const ForwardPlusPipeline&) -> ForwardPlusPipelineSubsystem;
} // namespace Ame::Gfx
