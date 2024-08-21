#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructGeometrySphereShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
#ifndef AME_GEOMETRY_PLANES_SHADER
#define AME_GEOMETRY_PLANES_SHADER
struct Sphere{float3 o; float r;};
Sphere Geometry_ComputeSphere(float3 center, float radius)
{
    Sphere s;
	s.o = center;
	s.r = radius;
	return s;
}
#endif
)";

    public:
        static constexpr const char* Name = "Structs/Geometry/Sphere.hlsli";

        StructGeometrySphereShader()
        {
            Setup({ "SG_Sphere", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi