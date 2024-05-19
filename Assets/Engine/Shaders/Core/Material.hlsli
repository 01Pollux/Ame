#ifndef AME_CORE_MATERIAL_H
#define AME_CORE_MATERIAL_H

#include "../Core/Ame.hlsli"
#include "../Core/FrameData.hlsli"
#include "../Core/RenderInstance.hlsli"
#include "../Geometry/Transform.hlsli"

struct InstanceInfo
{
	uint InstanceIndex;
};

struct VSInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tagent : TANGENT;
	float2 TexCoord : TEXCOORD;
};

#define MATERIALDATA_REGISTERSPACE 20

#define AME_MATERIAL_RESOURCE(type, name, regType, bindingIndex) AME_RESOURCE(type, name, regType, bindingIndex, 20)

AME_RESOURCE(StructuredBuffer<Transform>, g_Transforms, t, 0, 2);
AME_RESOURCE(StructuredBuffer<RenderInstance>, g_RenderInstances, t, 1, 2);
AME_PUSH_CONSTANT(InstanceInfo, g_InstanceInfo, 0);

#endif