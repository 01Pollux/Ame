#pragma once

#include <RG/Resource.hpp>

namespace Ame::RG::Names
{
    static inline const String c_BackbufferImageName{ "_BackbufferOut" };

    static inline const ResourceId c_FrameResource("_FrameResource");
    static inline const ResourceId c_BackbufferImage(c_BackbufferImageName);

    static inline const ResourceViewId c_FrameResourceMainView = c_FrameResource("Main");
} // namespace Ame::RG::Names