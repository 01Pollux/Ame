#pragma once

#include <Core/Subsystem.hpp>

#include <Ecs/Subsystem/Universe.hpp>
#include <Gfx/Subsystem/CommonPipelineState.hpp>
#include <Gfx/Subsystem/CommonShader.hpp>
#include <Gfx/Subsystem/MaterialBindingCache.hpp>
#include <Gfx/Subsystem/Pipelines/BaseGraphPipeline.hpp>

#include <Gfx/RG/Pipelines/DeferredPlusPipeline.hpp>

namespace Ame::Gfx::RG
{
    struct DeferredPlusPipelineSubsystem : SingleSubsystem<
                                               DeferredPlusPipeline,
                                               Dependency<
                                                   RG::GraphSubsystem,
                                                   Ecs::UniverseSubsystem,
                                                   Cache::CommonPipelineStateSubsystem,
                                                   Cache::CommonShaderSubsystem,
                                                   Cache::MaterialBindingCacheSubsystem>>,
                                           kgr::overrides<GraphRendererSubsystem>,
                                           kgr::final
    {
    };

    auto service_map(const DeferredPlusPipeline&) -> DeferredPlusPipelineSubsystem;
} // namespace Ame::Gfx::RG
