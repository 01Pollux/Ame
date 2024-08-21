#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructGeometryFrustumPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
#ifndef AME_GEOMETRY_FRUSTUMPLANES
#define AME_GEOMETRY_FRUSTUMPLANES
#define FRUSTUM_PLANE_COUNT 6
#define FRUSTUM_CORNER_COUNT 8
#define FRUSTUM_EDGE_COUNT 12
struct Frustum{float3 o; float r; float4 q; float l; float t; float b; float n; float f;};
#endif
)";

    public:
        static constexpr const char* Name = "Structs/Geometry/Frustum.hlsli";

        StructGeometryFrustumPlanesShader()
        {
            Setup({ "SG_FrustumPlanes", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi