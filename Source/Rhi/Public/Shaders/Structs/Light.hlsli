#ifndef STRUCT_LIGHT
#define STRUCT_LIGHT
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define LIGHT_TYPE_MASK 3
struct Light
{
	float4 color;
	float3 attenuation_angle;
	float range;
	uint transform_id;
	uint flags;
};
StructuredBuffer<Light> AllLightInstances; // All light instances
StructuredBuffer<uint> AllLightIndices; // All light indices
StructuredBuffer<uint> LinkedLightIndices; // [0]: grid_size for each chunk, [..]: light index
Texture2D<uint2> LightGrid; // x: offset, y: count

int light_get_type(const in Light light)
{
	return (light.flags & LIGHT_TYPE_MASK);
}
#endif
