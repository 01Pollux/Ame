#pragma once

#include <Shaders/EngineShader.hpp>

#include <Shaders/Structs/Transform.hpp>
#include <Shaders/Structs/RenderInstance.hpp>
#include <Shaders/Structs/CameraFrameData.hpp>

#include <Shaders/Structs/Inputs/StdVertexInput.hpp>
#include <Shaders/Structs/Inputs/StdPixelInput.hpp>

#include <Shaders/Structs/Outputs/StdVertexOutput.hpp>
#include <Shaders/Structs/Outputs/StdMaterialFragment.hpp>
#include <Shaders/Structs/Outputs/ForwardPlus_PixelOutput.hpp>

namespace Ame::Rhi
{
    class StructGeometryPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
#ifndef AME_GEOMETRY_PLANE
#define AME_GEOMETRY_PLANE
struct Plane{float3 n;float d;};
Plane Geometry_ComputePlane(float3 p0, float3 p1, float3 p2)
{
    Plane p;
	p.n = normalize(cross(p1 - p0, p2 - p0));
	p.d = -dot(p.n, p0);
	return p;
}
#endif
)";

    public:
        static constexpr const char* Name = "Structs/Geometry/Plane.hlsli";

        StructGeometryPlanesShader()
        {
            Setup({ "SG_Plane", Dg::SHADER_TYPE_UNKNOWN}, c_SourceCode);
        }
    };
} // namespace Ame::Rhi