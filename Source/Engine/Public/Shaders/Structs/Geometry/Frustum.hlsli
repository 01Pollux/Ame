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
Frustum Geometry_ComputeFrustum(const in float3 position, const in float4 orientation,
                                const in float r, const in float l, 
                                const in float t, const in float b, 
                                const in float n, const in float f)
{
	Frustum frustum;
	frustum.o = position;
	frustum.r = r;
	frustum.q = orientation;
	frustum.l = l;
	frustum.t = t;
	frustum.b = b;
	frustum.n = n;
	frustum.f = f;
	return frustum;
}

Frustum Geometry_ComputeFrustum(const in float4x4 InvProjection, const in bool rhcoords)
{
	Frustum Out;
    // Corners of the projection frustum in homogenous space.
    static const float4 HomogenousPoints[6] =
    {
        float4(1.0f, 0.0f, 1.0f, 1.0f),   // right (at far plane)
        float4(-1.0f, 0.0f, 1.0f, 1.0f),   // left
        float4(0.0f, 1.0f, 1.0f, 1.0f),   // top
        float4(0.0f, -1.0f, 1.0f, 1.0f),   // bottom
        float4(0.0f, 0.0f, 0.0f, 1.0f),     // near
        float4(0.0f, 0.0f, 1.0f, 1.0f)      // far
    };

    // Compute the frustum corners in world space.
    float4 Points[6];

    for (uint i = 0; i < 6; ++i)
    {
        // Transform point.
		Points[i] = mul(HomogenousPoints[i], InvProjection);
	}

    Out.o = float3(0.0f, 0.0f, 0.0f);
    Out.q = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // Compute the slopes.
    Points[0] = Points[0] * (1.0f / Points[0].z);
    Points[1] = Points[1] * (1.0f / Points[1].z);
    Points[2] = Points[2] * (1.0f / Points[2].z);
    Points[3] = Points[3] * (1.0f / Points[3].z);

    Out.r = Points[0].x;
    Out.l = Points[1].x;
    Out.t = Points[2].y;
    Out.b = Points[3].y;

    // Compute near and far.
    Points[4] = Points[4] * (1.0f / Points[4].w);
    Points[5] = Points[5] * (1.0f / Points[5].w);

    if (rhcoords)
    {
        Out.n = Points[5].z;
        Out.f = Points[4].z;
    }
    else
    {
        Out.n = Points[4].z;
        Out.f = Points[5].z;
    }
	return Out;
}

void Geometry_ComputeFrustumSubView(inout Frustum frustum, const in float2 divs, const in float2 offset)
{
    float delta_x = (frustum.r - frustum.l) / divs.x;
    float delta_y = (frustum.t - frustum.b) / divs.y;
    
    frustum.l = frustum.l + offset.x * delta_x;
    frustum.r = frustum.l + delta_x;
    frustum.b = frustum.b + offset.y * delta_y;
    frustum.t = frustum.b + delta_y;
}

#endif