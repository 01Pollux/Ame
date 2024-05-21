#pragma once

namespace Ame::FlappyRocket
{
    static constexpr const char* s_ShaderSource = R"(
#include "../Core/Material.hlsli"

//

AME_MATERIAL_RESOURCE(Texture2D<float4>, p_BasecolorMap, t, 0);
AME_MATERIAL_RESOURCE(SamplerState, p_Sampler, s, 1);

//

[shader("vertex")]
Ecs_PSInput VS_Main(
	Ecs_VSInput input)
{
	Ecs_PSInput output = (Ecs_PSInput) 0;
	
	RenderInstance instance = g_RenderInstances[g_InstanceInfo.InstanceIndex];
	Transform transform = g_Transforms[instance.TransformIndex];
	
	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, transform.World);
	
	output.NormalWS = mul(float4(input.Normal, 0.0f), transform.World).xyz;
	output.TangentWS = mul(float4(input.Tangent, 0.0f), transform.World).xyz;
	output.BitangentWS = normalize(cross(output.NormalWS, output.TangentWS));

	output.TexCoord = input.TexCoord;
	
	return output;
}

export MaterialFragment PS_Main(
	Ecs_PSInput input)
{
	MaterialFragment fragment = (MaterialFragment) 0;
	
	//fragment.BaseColor = p_BasecolorMap.Sample(p_Sampler, input.TexCoord).xyz;
	fragment.BaseColor = float3(1.0f, 1.0f, 1.0f);
	
	return fragment;
}
)";
} // namespace Ame::FlappyRocket
