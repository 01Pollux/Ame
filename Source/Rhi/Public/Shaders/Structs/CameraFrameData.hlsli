#ifndef STRUCT_FRAMEDATA
#define STRUCT_FRAMEDATA
struct CameraFrameData
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 ViewProjection;
	float4x4 ViewInverse;
	float4x4 ProjectionInverse;
	float4x4 ViewProjectionInverse;
	float2 Viewport;
	float EngineTime;
	float GameTime;
	float DeltaTime;
	float NearPlane;
	float FarPlane;
	uint CameraMask;
};
#endif