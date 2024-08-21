#ifndef AME_GEOMETRY_PLANE
#define AME_GEOMETRY_PLANE
struct Plane
{
	float3 n;
	float d;
};
Plane Geometry_ComputePlane(float3 p0, float3 p1, float3 p2)
{
	Plane p;
	p.n = normalize(cross(p1 - p0, p2 - p0));
	p.d = -dot(p.n, p0);
	return p;
}
#endif