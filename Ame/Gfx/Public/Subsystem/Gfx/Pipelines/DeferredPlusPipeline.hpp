#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Ecs/Universe.hpp>
#include <Subsystem/Gfx/CommonPipelineState.hpp>
#include <Subsystem/Gfx/CommonShader.hpp>
#include <Subsystem/Gfx/MaterialBindingCache.hpp>
#include <Subsystem/Gfx/Pipelines/BaseGraphPipeline.hpp>

#include <Gfx/RenderGraph/Pipelines/DeferredPlusPipeline.hpp>

namespace Ame::Gfx
{
    struct DeferredPlusPipelineSubsystem : SingleSubsystem<
                                               DeferredPlusPipeline,
                                               Dependency<
                                                   RG::GraphSubsystem,
                                                   Ecs::UniverseSubsystem,
                                                   Cache::CommonShaderSubsystem,
                                                   Cache::CommonPipelineStateSubsystem,
                                                   Cache::MaterialBindingCacheSubsystem>>,
                                           kgr::overrides<GraphRendererSubsystem>,
                                           kgr::final
    {
    };

    auto service_map(const DeferredPlusPipeline&) -> DeferredPlusPipelineSubsystem;
} // namespace Ame::Gfx
