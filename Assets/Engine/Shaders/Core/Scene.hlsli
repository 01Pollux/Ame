#ifndef AME_CORE_SCENE_H
#define AME_CORE_SCENE_H

struct VSInput_Scene
{
	float3 LS_Position : POSITION;
	float3 LS_Normal : NORMAL;
	float3 LS_Tangent : TANGENT;
	float2 TexCoord : TEXCOORD;
};

struct PSInput_Scene
{
	float4 ScreenPosition : SV_POSITION;
	float3 WS_Position : WORLD_POSITION;
	float3 WS_Normal : WORLD_NORMAL;
	float3 WS_Tangent : WORLD_TANGENT;
	float3 WS_BiTangent : WORLD_BITANGENT;
	float2 TexCoord : TEXCOORD;
};

#endif