#ifndef AME_UTILS_VIEW_CONVERTER_SHADER
#define AME_UTILS_VIEW_CONVERTER_SHADER
float4 Utils_ClipToView(const in float4 clip, const in float4x4 inv_projection)
{
	float4 view = mul(inv_projection, clip);
	view = view / view.w;
	return view;
}
float4 Utils_ScreenToView(const in float4 screen, const in float2 screen_dims, const in float4x4 inv_projection)
{
	float2 tex_coord = screen.xy / screen_dims;
	float4 clip = float4(float2(tex_coord.x, 1.0f - tex_coord.y) * 2.0f - 1.0f, screen.z, screen.w);
	return Utils_ClipToView(clip, inv_projection);
}
#endif