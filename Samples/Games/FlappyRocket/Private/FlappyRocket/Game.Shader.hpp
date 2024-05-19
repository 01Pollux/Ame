#pragma once

namespace Ame::FlappyRocket
{
    static constexpr const char* s_ShaderSource = R"(
#include "../Core/Material.hlsli"

//

struct VSOutput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

typedef VSOutput PSInput;

//

AME_MATERIAL_RESOURCE(Texture2D<float4>, p_Albedo, t, 0);
AME_MATERIAL_RESOURCE(SamplerState, p_Sampler, s, 1);

//

VSOutput VS_Main(
	VSInput input)
{
	VSOutput output = (VSOutput) 0;
	
	RenderInstance instance = g_RenderInstances[g_InstanceInfo.InstanceIndex];
	Transform transform = g_Transforms[instance.TransformIndex];
	
	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, transform.World);
	
	output.Normal = input.Normal;
	output.TexCoord = input.TexCoord;
	
	return output;
}

float4 PS_Main(
	PSInput input) : SV_TARGET
{
	//float4 albedo = p_Albedo.Sample(p_Sampler, input.TexCoord);
	//return albedo;
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
)";
} // namespace Ame::FlappyRocket
