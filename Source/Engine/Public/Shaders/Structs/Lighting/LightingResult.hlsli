#ifndef AME_LIGHTING_RESULT
#define AME_LIGHTING_RESULT
struct LightingResult
{
	float3 diffuse;
	float3 specular;
};
struct LightingSurface
{
	float3 position;
	float3 view_dir;
	float specular_power;
	float3 normal;
};
#endif