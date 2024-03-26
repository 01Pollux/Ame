#include "../Core/Scene.hlsli"
#include "../Core/FrameData.hlsli"
#include "../Core/InstanceData.hlsli"

//

struct DrawData
{
	uint InstanceIndex;
};

AME_PUSH_CONSTANT(DrawData, g_DrawData, 0);

//

AME_RESOURCE(Texture2D<float4>, p_Albedo, t, 0, 2);
AME_RESOURCE(SamplerState, p_Sampler, s, 0, 2);

//

PSInput_Scene VS_Main(
	VSInput_Scene Input)
{
	PSInput_Scene Output;

	float4x4 WorldMatrix = g_InstanceData[g_DrawData.InstanceIndex].GetWorldMatrix();
	
	Output.WS_Position = mul(float4(Input.LS_Position, 1.f), WorldMatrix).xyz;
	Output.WS_Normal = mul(float4(Input.LS_Normal, 0.f), WorldMatrix).xyz;
	Output.WS_Tangent = mul(float4(Input.LS_Tangent, 0.f), WorldMatrix).xyz;
	Output.WS_BiTangent = normalize(cross(Output.WS_Normal, Output.WS_Tangent));
	Output.TexCoord = Input.TexCoord;
	
	Output.ScreenPosition = mul(float4(Output.WS_Position, 1.0f), g_FrameData.ViewProjection);
	return Output;
}

float4 PS_Main(
	PSInput_Scene Input) : SV_TARGET
{
	float4 Albedo = p_Albedo.Sample(p_Sampler, Input.TexCoord);
	return Albedo;
}