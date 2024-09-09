#ifndef AME_LIGHTING_OPERATIONS
#define AME_LIGHTING_OPERATIONS
void LightingResult_Combine(inout LightingResult result, const in LightingResult other)
{
	result.diffuse += other.diffuse;
	result.specular += other.specular;
}
void LightingResult_Compute(inout LightingResult result, const in Light light, const in float diffuse, const in float specular)
{
	result.diffuse = diffuse * light.color;
	result.specular = specular * light.color;
}

//

float Lighting_DiffuseLambert(const in float3 normal, const in float3 light_dir)
{
	return max(dot(light_dir, normal), 0.f);
}

float Lighting_SpecularBlinnPhong(
	const in float3 normal, const in float3 light_dir,
	const in float3 view_dir, const in float specular_power)
{
	float3 reflect_dir = normalize(reflect(-light_dir, normal));
	return pow(max(dot(reflect_dir, view_dir), 0.f), specular_power);
}

//

void _LightingResult_Compute_Directional(
	inout LightingResult result, const in Transform light_transform,
	const in Light light, const in LightingSurface surface)
{
	float3 light_dir = normalize(-transform_get_forward(light_transform));
	float diffuse = Lighting_DiffuseLambert(surface.normal, light_dir);
	float specular = 0.f;
	if (surface.specular_power > 0.f)
	{
		specular = Lighting_SpecularBlinnPhong(surface.normal, light_dir, surface.view_dir, surface.specular_power);
	}
	LightingResult_Compute(result, light, diffuse, specular);
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
	uint block_size = LinkedLightIndices[LIGHT_BLOCK_SIZE_OFFSET];
	uint2 tile_index = uint2(floor(uv / block_size));
	uint2 count_offset = LightGrid[tile_index];
	uint count = count_offset.x;
	uint offset = count_offset.y;

	LightingResult tmp;
	for (uint i = 0; i < count; i++)
	{
		uint li = LinkedLightIndices[LIGHT_INDEX_OFFSET(offset + i)];
		Light light = AllLightInstances[li];
		Transform light_transform = AllTransforms[light.transform_id];
		
		LightingResult_Compute(tmp, light_transform, light, surface);
		LightingResult_Combine(result, tmp);
	}
}

#endif