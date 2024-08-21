Texture2D LightCountHeatMap;
RWTexture2D<float4> DebugTexture;
void write_debug_texture(uint gid, uint light_count)
{
	if (gid.x == 0 || gid.y == 0)
	{
		DebugTexture[gid.xy] = float4(0, 0, 0, 0.9f);
	}
	else if (gid.x == 1 || gid.y == 1)
	{
		DebugTexture[gid.xy] = float4(1, 1, 1, 0.5f);
	}
	else if (light_count > 0)
	{
		float normalized_light_count = light_count / LightCountHeatMap.GetDimensions().x;
		float4 color = LightCountHeatMap.SampleLevel(samLinearClamp, float2(normalized_light_count, 0), 0);
		DebugTexture[gid.xy] = color;
	}
	else
	{
		DebugTexture[gid.xy] = float4(0, 0, 0, 1);
	}
}
