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
    class GeometryPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
#ifndef AME_GEOMETRY_CONE
#define AME_GEOMETRY_CONE
struct Cone{float3 o; float r;};
Cone Geometry_ComputeCone(float3 center, float radius)
{
    Cone c;
	c.o = center;
	c.r = radius;
	return c;
}
#endif
)";

    public:
        static constexpr const char* Name = "Structs/Geometry/Planes.hlsli";

        GeometryPlanesShader()
        {
            Setup({ "G_Planes", Dg::SHADER_TYPE_UNKNOWN}, c_SourceCode);
        }
    };
} // namespace Ame::Rhi