#pragma once

#include <Gfx/RG/Resource.hpp>

namespace Ame::Gfx::RG::Names
{
    static inline const ResourceId OutputImage("_OutputImage");
    static inline const ResourceId FrameResource("_FrameResource");
    static inline const ResourceId TransformsTable("_TransformsTable");
    static inline const ResourceId RenderInstancesTable("_RenderInstancesTable");

    static inline const ResourceId EntityCommandCounter("_EntityCommandCounter");
    static inline const ResourceId EntityCommandBuffer("_EntityCommandBuffer");

    //

    static inline const ResourceViewId FrameResourceMainView = FrameResource("Main");
} // namespace Ame::Gfx::RG::Names