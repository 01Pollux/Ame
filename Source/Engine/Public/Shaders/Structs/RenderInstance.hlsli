#ifndef STRUCT_RENDERINSTANCE
#define STRUCT_RENDERINSTANCE
struct RenderInstance
{
	uint transform_id;
	uint bounding_id;
	uint code;
};
StructuredBuffer<RenderInstance> AllRenderInstances;

#endif