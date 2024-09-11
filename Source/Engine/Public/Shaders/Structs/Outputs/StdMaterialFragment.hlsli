#ifndef STRUCT_STD_MATERIAL_FRAGMENT
#define STRUCT_STD_MATERIAL_FRAGMENT
struct material_fragment
{
	float4 base_color;
	float3 normal;
	float ao;
	float4 emissive;
	float roughness;
	float specular;
	float metallic;
	float height;
};
#endif