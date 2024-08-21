#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructLightShader : public EngineShader
    {
    private:
        // clang-format off
        static constexpr const char c_SourceCode[] = R"(
#ifndef STRUCT_LIGHT
#define STRUCT_LIGHT
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define LIGHT_TYPE_MASK 3
struct Light{float4 color; float3 attenuation_angle; float range; uint transform_id; uint flags;};
int light_get_type(const in Light light){return (light.flags & LIGHT_TYPE_MASK);}
#endif
)";
        // clang-format on

    public:
        static constexpr const char* Name = "Structs/Light.hlsli";

        StructLightShader()
        {
            Setup({ "S_Light", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi