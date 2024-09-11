#ifndef STRUCT_STD_VERTEX_OUTPUT
#define STRUCT_STD_VERTEX_OUTPUT
struct vs_output
{
	float4 screen_position : SV_POSITION;
	float3 world_position : WORLD_POSITION;
	float3 world_normal : WORLD_NORMAL;
	float2 tex_coord : TEXCOORD;
	float3 world_tangent : WORLD_TANGENT;
};
struct vs_output_pos_only
{
	float4 screen_position : SV_POSITION;
};
#endif