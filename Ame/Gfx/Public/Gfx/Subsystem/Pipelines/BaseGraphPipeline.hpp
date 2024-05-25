#pragma once

#include <Core/Subsystem.hpp>

#include <Gfx/Subsystem/Graph.hpp>

#include <Gfx/RG/Pipelines/BaseGraphPipeline.hpp>

namespace Ame::Gfx::RG
{
    class DeferredPlusPipelineSubsystem;

    struct GraphRendererSubsystem : AbstractSubsystem<IBaseGraphPipeline>,
                                    kgr::defaults_to<DeferredPlusPipelineSubsystem>
    {
    };
} // namespace Ame::Gfx::RG
