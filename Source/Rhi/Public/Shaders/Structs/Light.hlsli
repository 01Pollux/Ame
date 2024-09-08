#ifndef STRUCT_LIGHT
#define STRUCT_LIGHT

// Relative to LinkedLightIndices, where first element is block size, second is total light count in LinkedLightIndices, and the rest are light indices
#define LIGHT_BLOCK_SIZE_OFFSET 0
#define LIGHT_COUNT_OFFSET 1
#define LIGHT_INDEX_OFFSET(i) (2 + i)

#define LIGHT_TYPE_DIRECTIONAL (1 << 0)
#define LIGHT_TYPE_POINT (1 << 1)
#define LIGHT_TYPE_SPOT (1 << 2)
#define LIGHT_TYPE_MASK (LIGHT_TYPE_DIRECTIONAL | LIGHT_TYPE_POINT | LIGHT_TYPE_SPOT)

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
