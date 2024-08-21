#ifndef STRUCT_STD_MATERIAL_FRAGMENT
#define STRUCT_STD_MATERIAL_FRAGMENT
struct material_fragment
{
	float4 base_color;
	float3 normal;
	float ao;
	float3 specular;
	float4 emissive;
	float roughness;
	float3 shininess;
	float metallic;
	float height;
};
#endif