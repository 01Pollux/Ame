#ifndef STRUCT_STD_PIXEL_INPUT
#define STRUCT_STD_PIXEL_INPUT
struct ps_input
{
	float4 screen_position : SV_POSITION;
	float3 world_position : WORLD_POSITION;
	float3 world_normal : WORLD_NORMAL;
	float2 tex_coord : TEXCOORD;
	float3 world_tangent : WORLD_TANGENT;
};
#endif