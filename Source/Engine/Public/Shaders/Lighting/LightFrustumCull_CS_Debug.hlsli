RWTexture2D<float4> DebugTexture;
void write_debug_texture(uint2 gid, uint light_count)
{
	if (light_count > 0)
	{
		float normalized_light_count = light_count / MAX_LIGHT_CHUNK_SIZE;
		float3 color = lerp(float3(0, 1, 0), float3(1, 0.5, 0), normalized_light_count);
		DebugTexture[gid.xy] = float4(color, 1.f);
	}
	else
	{
		DebugTexture[gid.xy] = float4(0, 0, 0, 1);
	}
}
