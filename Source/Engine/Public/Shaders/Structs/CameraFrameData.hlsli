#ifndef STRUCT_FRAMEDATA
#define STRUCT_FRAMEDATA
struct CameraFrameData
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
	float4x4 view_projection;
	float4x4 view_inverse;
	float4x4 projection_inverse;
	float4x4 view_projection_inverse;
	float2 viewport;
	float engine_time;
	float game_time;
	float delta_time;
	float near_plane;
	float far_plane;
	uint camera_mask;
};
cbuffer FrameDataBuffer
{
	CameraFrameData FrameData;
};

float3 FrameData_GetPosition(const in CameraFrameData frame_data)
{
	return frame_data.world[3].xyz;
}
#endif