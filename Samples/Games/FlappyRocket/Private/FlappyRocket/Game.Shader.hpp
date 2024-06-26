#pragma once

namespace Ame::FlappyRocket
{
    static constexpr const char* c_ShaderSource = R"(
#include "../Core/Material.hlsli"

struct InstanceInfo
{
	uint InstanceId;
};
AME_PUSH_CONSTANT(InstanceInfo, g_RenderInstance, 0);

//

struct UserData
{
	float4 _Color;
};
AME_MATERIAL_USERDATA(UserData);

AME_MATERIAL_RESOURCE(SamplerState, _Sampler, s, 1);
AME_MATERIAL_RESOURCE(Texture2D<float4>, _Texture, t, 2);

//

[shader("vertex")]
Ecs_PSInput VS_Main(
	Ecs_VSInput vertex,
	AME_DECLARE_DRAW_PARAMETERS)
{
	Ecs_PSInput output = (Ecs_PSInput) 0;
	
	RenderInstance instance = g_RenderInstances[g_RenderInstance.InstanceId];
	Transform transform = g_Transforms[instance.TransformId];
	
	float4 PositionWS = float4(vertex.Position, 1.0f);
	PositionWS = mul(PositionWS, transform.World);
	output.Position = mul(PositionWS, g_FrameInfo.ViewProjection);
	output.PositionWS = PositionWS.xyz;
	
	output.NormalWS = mul(transform.World, float4(vertex.Normal, 0.0f)).xyz;
	output.TangentWS = mul(transform.World, float4(vertex.Tangent, 0.0f)).xyz;
	output.BitangentWS = normalize(cross(output.NormalWS, output.TangentWS));

	output.TexCoord = vertex.TexCoord;
	
	return output;
}

AME_EXPORT MaterialFragment PSM_Main(
	Ecs_PSInput input)
{
	MaterialFragment fragment = (MaterialFragment) 0;
	
	//fragment.BaseColor = _UserData._Color.xyz;
	fragment.BaseColor = (_Texture.Sample(_Sampler, input.TexCoord) * _UserData._Color).xyz;
	
	return fragment;
}
)";
} // namespace Ame::FlappyRocket
