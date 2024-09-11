#ifndef STRUCT_TRANSFORM
#define STRUCT_TRANSFORM
struct Transform
{
	float4x4 World;
};
StructuredBuffer<Transform> AllTransforms;

float3 transform_get_position(const in Transform transform)
{
	return transform.World[3].xyz;
}
float3 transform_get_forward(const in Transform transform)
{
	return transform.World[2].xyz;
}
float3 transform_get_up(const in Transform transform)
{
	return transform.World[1].xyz;
}
float3 transform_get_right(const in Transform transform)
{
	return transform.World[0].xyz;
}
#endif