#ifndef AME_LIGHTING_OPERATIONS
#define AME_LIGHTING_OPERATIONS
void LightingResult_Combine(inout LightingResult result, const in LightingResult other)
{
	result.diffuse += other.diffuse;
	result.specular += other.specular;
}
void LightingResult_Compute(inout LightingResult result, const in float4 diffuse, const in float4 specular)
{
	result.diffuse = diffuse;
	result.specular = specular;
}

//

float Lighting_DiffuseLambert(const in float3 normal, const in float3 light_dir)
{
	return saturate(dot(normal, light_dir));
}

float Lighting_SpecularBlinnPhong(
	const in float3 normal, const in float3 light_dir,
	const in float3 view_dir, const in float specular_power)
{
	float3 half_dir = normalize(light_dir + view_dir);
	return pow(saturate(dot(normal, half_dir)), specular_power);
}

//

void _LightingResult_Compute_Directional(
	inout LightingResult result, const in Transform light_transform,
	const in Light light, const in LightingSurface surface)
{
	float3 light_dir = normalize(-transform_get_forward(light_transform));
	float4 diffuse = Lighting_DiffuseLambert(surface.normal, light_dir) * light.color;
	float4 specular = Lighting_SpecularBlinnPhong(surface.normal, light_dir, surface.view_dir, surface.specular_power) * light.color;
	LightingResult_Compute(result, diffuse, specular);
}

void LightingResult_Compute(
	inout LightingResult result, const in Transform light_transform,
	const in Light light, const in LightingSurface surface)
{
	switch (light_get_type(light))
	{
	case LIGHT_TYPE_DIRECTIONAL:
	{
		_LightingResult_Compute_Directional(result, light_transform, light, surface);
		break;
	}
	}
}

void LightingResult_ComputeFromLinkedList(
	inout LightingResult result,
	const in float2 uv, const in LightingSurface surface)
{
	uint block_size = LinkedLightIndices[0];
	uint2 tile_index = uint2(floor(uv / block_size));
	uint2 offset_count = LightGrid[tile_index];
	uint offset = offset_count.x;
	uint count = offset_count.y;

	LightingResult tmp;
	for (uint i = 0; i < count; i++)
	{
		uint li = LinkedLightIndices[offset + i + 1];
		Light light = AllLightInstances[li];
		Transform light_transform = AllTransforms[light.transform_id];
		
		LightingResult_Compute(tmp, light_transform, light, surface);
		LightingResult_Combine(result, tmp);
	}
}

#endif