#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructStdMaterial3D_DataShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode = R"(
	#ifndef STRUCT_STD_MATERIAL_3D_DATA
	#define STRUCT_STD_MATERIAL_3D_DATA
// must be same as in Shading/Types/StandardMaterial3D.hpp
Texture2D<float4> BaseColorMap;
Texture2D<float3> NormalMap;
Texture2D<float> AOMap;
Texture2D<float3> SpecularMap;
Texture2D<float4> EmissiveMap;
Texture2D<float3> ShininessMap;
Texture2D<float2> Roughness_MetallicMap;
Texture2D HeightMap;

	#endif
)";

    public:
        static constexpr const char* Name = "Structs/Rendering/StdMaterial3D_Data.hlsli";

        StructStdMaterial3D_DataShader()
        {
            Setup({ "S_StdMaterialFragment", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi