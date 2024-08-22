#pragma once

#include <Rg/Resource.hpp>

namespace Ame::Gfx
{
    static inline const Rg::ResourceId c_RGFinalImage{ "" };
    static inline const Rg::ResourceId c_RGDepthImage{ "_Depth" };

    //

    static inline const Rg::ResourceId c_RGFrameData{ "_FrameData" }; // cbuffer FrameDataBuffer

    static inline const Rg::ResourceId c_RGTransformTable{ "_Transforms" };             // StructuredBuffer<Transform> Transforms
    static inline const Rg::ResourceId c_RGRenderInstanceTable{ "_RenderInstances" };   // StructuredBuffer<RenderInstance> RenderInstances
    static inline const Rg::ResourceId c_RGLightInstanceTable{ "_LightInstances" };     // StructuredBuffer<Light> LightInstances
    static inline const Rg::ResourceId c_RGLightIdInstanceTable{ "_LightIdInstances" }; // StructuredBuffer<uint> LightIndices

    //

    //

    static inline const Rg::ResourceId c_RGEntityResourceSignature_Graphics{ "_ERS_Graphics" }; // Srb: [FrameDataBuffer, Transforms, RenderInstances, LightInstances, Samplers]
    static inline const Rg::ResourceId c_RGEntityResourceSignature_Compute{ "_ERS_Compute" };   // Srb: [FrameDataBuffer, Transforms, RenderInstances, LightInstances, Samplers]

    static inline const Rg::ResourceId c_RGEntityEmptyVertexBuffers{ "_EEVB" };
} // namespace Ame::Gfx