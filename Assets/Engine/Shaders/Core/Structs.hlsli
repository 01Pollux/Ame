#ifndef AME_CORE_STRUCTS_H
#define AME_CORE_STRUCTS_H

#include "Ame.hlsli"

struct Ecs_VSInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 TexCoord : TEXCOORD;
};

struct Ecs_PSInput
{
	float4 Position : SV_POSITION;
	float3 PositionWS : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalWS : NORMAL;
	float3 TangentWS : TANGENT;
	float3 BitangentWS : BITANGENT;
};

struct MaterialFragment
{
	float3 BaseColor;
	float4 Normal;
	float4 Emissive;
	float Roughness;
	float Metallic;
};

#endif