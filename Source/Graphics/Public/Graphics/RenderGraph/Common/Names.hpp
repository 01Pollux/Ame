#pragma once

#include <RG/Resource.hpp>

namespace Ame::Gfx
{
    static inline const RG::ResourceId c_RGFrameData{ "_FrameData" };

    static inline const RG::ResourceId c_RGFinalImage{ "" };
    static inline const RG::ResourceId c_RGDepthImage{ "_Depth" };

    static inline const RG::ResourceId c_RGEntityResourceSignature_Graphics{ "_ERS_Graphics" };
    static inline const RG::ResourceId c_RGEntityEmptyVertexBuffers{ "_EEVB" };

    static inline const RG::ResourceId c_RGTransformTable{ "_Transforms" };
    static inline const RG::ResourceId c_RGRenderInstanceTable{ "_RenderInstances" };
} // namespace Ame::Gfx