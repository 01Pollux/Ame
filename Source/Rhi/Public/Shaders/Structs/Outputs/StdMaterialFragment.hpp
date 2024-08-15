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
    struct material_fragment{float3 albedo;float3 normal;float3 tangent;float3 bitangent;float3 emissive;float roughness;float metallic;float ao;};
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