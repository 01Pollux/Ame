#pragma once

#include <Rg/Resource.hpp>

namespace Ame::Gfx
{
    inline const Rg::ResourceId c_RGFinalImage{ "_Image" };
    inline const Rg::ResourceId c_RGDepthImage{ "_Depth" };

    //

    inline const Rg::ResourceId c_RGFrameData{ "_FrameData" }; // cbuffer FrameDataBuffer

    inline const Rg::ResourceId c_RGTransformTable{ "_Transforms" };             // StructuredBuffer<Transform> Transforms
    inline const Rg::ResourceId c_RGRenderInstanceTable{ "_RenderInstances" };   // StructuredBuffer<RenderInstance> RenderInstances
    inline const Rg::ResourceId c_RGLightInstanceTable{ "_LightInstances" };     // StructuredBuffer<Light> LightInstances
    inline const Rg::ResourceId c_RGLightIdInstanceTable{ "_LightIdInstances" }; // StructuredBuffer<uint> LightIndices

    //

    inline const Rg::ResourceId c_RGLightIndices_Transparent{ "_LightIndices_Transparent" }; // RWStructuredBuffer<uint>
    inline const Rg::ResourceId c_RGLightIndices_Opaque{ "_LightIndices_Opaque" };           // RWStructuredBuffer<uint>

    inline const Rg::ResourceId c_RGLightHeads_Transparent{ "_LightHeads_Transparent" }; // RWTexture2D<uint2>
    inline const Rg::ResourceId c_RGLightHeads_Opaque{ "_LightHeads_Opaque" };           // RWTexture2D<uint2>

#ifndef AME_DIST
    inline const Rg::ResourceId c_RGLightDebugTextures{ "_LightDebugTextures" }; // RWTexture2D<float>
#endif

    //

    inline const Rg::ResourceId c_RGEntityResourceSignature_Graphics{ "_ERS_Graphics" }; // Srb: [FrameDataBuffer, Transforms, RenderInstances, LightInstances, Samplers]
    inline const Rg::ResourceId c_RGEntityResourceSignature_Compute{ "_ERS_Compute" };   // Srb: [FrameDataBuffer, Transforms, RenderInstances, LightInstances, Samplers]

    inline const Rg::ResourceId c_RGEntityEmptyVertexBuffers{ "_EEVB" };
} // namespace Ame::Gfx