#ifndef AME_GEOMETRY_PLANES_SHADER
#define AME_GEOMETRY_PLANES_SHADER
struct Sphere
{
	float3 o;
	float r;
};
Sphere Geometry_ComputeSphere(float3 center, float radius)
{
	Sphere s;
	s.o = center;
	s.r = radius;
	return s;
}
#endif