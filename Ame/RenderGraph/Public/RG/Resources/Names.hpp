#pragma once

#include <RG/Resource.hpp>

namespace Ame::RG::Names
{
    static inline const ResourceId c_OutputImage("_OutputImage");
    static inline const ResourceId c_FrameResource("_FrameResource");
    static inline const ResourceId c_TransformsTable("_TransformsTable");
    static inline const ResourceId c_RenderInstancesTable("_RenderInstancesTable");

    static inline const ResourceId c_EntityCommandCounter("_EntityCommandCounter");
    static inline const ResourceId c_EntityCommandBuffer("_EntityCommandBuffer");

    //

    static inline const ResourceViewId c_FrameResourceMainView = c_FrameResource("Main");
} // namespace Ame::RG::Names