#ifndef AME_GEOMETRY_FRUSTUMPLANES
#define AME_GEOMETRY_FRUSTUMPLANES
#define FRUSTUM_PLANE_COUNT 6
#define FRUSTUM_CORNER_COUNT 8
#define FRUSTUM_EDGE_COUNT 12
struct Frustum
{
	float3 o;
	float r;
	float4 q;
	float l;
	float t;
	float b;
	float n;
	float f;
};
#endif