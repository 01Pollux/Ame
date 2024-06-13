#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/RG/RenderGraph.hpp>

#include <Gfx/RenderGraph/Pipelines/BaseGraphPipeline.hpp>

namespace Ame::Gfx
{
    class DeferredPlusPipelineSubsystem;

    struct GraphRendererSubsystem : AbstractSubsystem<IBaseGraphPipeline>,
                                    kgr::defaults_to<DeferredPlusPipelineSubsystem>
    {
    };
} // namespace Ame::Gfx
