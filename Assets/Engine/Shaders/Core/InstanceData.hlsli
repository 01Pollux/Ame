#ifndef AME_CORE_INSTANCEDATA_H
#define AME_CORE_INSTANCEDATA_H

#include "../Core/Ame.hlsli"

struct InstanceData
{
	float4x4 World_Radius;
	
	float4x4 GetWorldMatrix()
	{
		float4x4 copy = World_Radius;
		copy[3][3] = 1.f;
		return copy;
	}
};

AME_RESOURCE(StructuredBuffer<InstanceData>, g_InstanceData, t, 0, 1);

#endif