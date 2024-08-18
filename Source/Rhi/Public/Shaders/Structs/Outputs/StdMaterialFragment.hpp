#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructStdMaterialFragmentShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode = R"(
	#ifndef STRUCT_STD_MATERIAL_FRAGMENT
	#define STRUCT_STD_MATERIAL_FRAGMENT
    struct material_fragment{float4 base_color;float3 normal;float ao;float3 specular;float4 emissive;float roughness;float3 shininess;float metallic;float height;};
	#endif
)";

    public:
        static constexpr const char* Name = "Structs/Outputs/StdMaterialFragment.hlsli";

        StructStdMaterialFragmentShader()
        {
            Setup({ "S_StdMaterialFragment", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi