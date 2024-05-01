#pragma once

#include <Gfx/RG/Resource.hpp>

namespace Ame::Gfx::RG::Names
{
    static inline const ResourceId OutputImage("_OutputImage");
    static inline const ResourceId FrameResource("_FrameResource");

    static inline const ResourceViewId FrameResourceMain = FrameResource("Main");
} // namespace Ame::Gfx::RG::Names