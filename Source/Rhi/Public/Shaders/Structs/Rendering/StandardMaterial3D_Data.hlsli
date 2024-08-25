#ifndef STRUCT_STD_MATERIAL_3D_DATA
#define STRUCT_STD_MATERIAL_3D_DATA
// must be same as in Shading/Types/StandardMaterial3D.hpp
Texture2D<float4> BaseColorMap;
Texture2D<float3> NormalMap;
Texture2D<float> AOMap;
Texture2D<float4> EmissiveMap;
Texture2D<float> SpecularMap;
Texture2D<float2> Roughness_MetallicMap;
Texture2D<float> HeightMap;
#endif