#ifndef AME_CORE_CAMERADATA_H
#define AME_CORE_CAMERADATA_H

#include "../Core/Ame.hlsli"

struct FrameDesc
{
	matrix World;
	
	matrix View;
	matrix Projection;
	matrix ViewProjection;
	
	matrix InverseView;
	matrix InverseProjection;
	matrix InverseViewProjection;
	
	float2 ScreenSize;
	float EngineTime;
	float GameTime;
	float DeltaTime;
	
	//
	
	float3 GetPosition()
	{
		return World._41_42_43;
	}
	
	float3 GetForward()
	{
		return -World._31_32_33;
	}
	
	float3 GetUp()
	{
		return World._21_22_23;
	}
	
	float3 GetRight()
	{
		return World._11_12_13;
	}
};

#endif