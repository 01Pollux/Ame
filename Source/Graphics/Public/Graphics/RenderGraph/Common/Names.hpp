#pragma once

#include <Rg/Resource.hpp>

namespace Ame::Gfx
{
    static inline const Rg::ResourceId c_RGFrameData{ "_FrameData" };

    static inline const Rg::ResourceId c_RGFinalImage{ "" };
    static inline const Rg::ResourceId c_RGDepthImage{ "_Depth" };

    static inline const Rg::ResourceId c_RGEntityResourceSignature_Graphics{ "_ERS_Graphics" };
    static inline const Rg::ResourceId c_RGEntityEmptyVertexBuffers{ "_EEVB" };

    static inline const Rg::ResourceId c_RGTransformTable{ "_Transforms" };
    static inline const Rg::ResourceId c_RGRenderInstanceTable{ "_RenderInstances" };
    static inline const Rg::ResourceId c_RGLightInstanceTable{ "_LightInstances" };
} // namespace Ame::Gfx