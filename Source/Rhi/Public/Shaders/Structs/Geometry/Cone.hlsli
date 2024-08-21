#ifndef AME_GEOMETRY_CONE
#define AME_GEOMETRY_CONE
struct Cone
{
	float3 o;
	float r;
};
Cone Geometry_ComputeCone(float3 center, float radius)
{
	Cone c;
	c.o = center;
	c.r = radius;
	return c;
}
#endif