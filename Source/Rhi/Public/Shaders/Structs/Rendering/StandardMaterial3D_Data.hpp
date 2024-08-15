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
Texture2D<float4> NormalMap;
Texture2D<float4> SpecularMap;
Texture2D<float4> RoughnessMap;
Texture2D<float4> AmbientMap;
Texture2D<float4> EmissiveMap;
Texture2D<float4> HeightMap;
Texture2D<float4> ShininessMap;
Texture2D<float4> OpacityMap;
Texture2D<float4> DisplacementMap;
Texture2D<float4> Lightmap;
Texture2D<float4> MetallicMap;
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