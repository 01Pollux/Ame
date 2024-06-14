#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/RG/RenderGraph.hpp>

#include <Gfx/RenderGraph/Pipelines/BaseGraphPipeline.hpp>

namespace Ame::Gfx
{
    class ForwardPlusPipelineSubsystem;

    struct GraphRendererSubsystem : AbstractSubsystem<IBaseGraphPipeline>,
                                    kgr::defaults_to<ForwardPlusPipelineSubsystem>
    {
    };
} // namespace Ame::Gfx
