
#include "../Core/Material.hlsli"

//

struct PSOutput
{
	float4 Normal3_Metallic1 : SV_Target0;
	float4 BaseColor3_Roughness1 : SV_Target1;
	float4 Emissive3_AO1 : SV_Target2;
};

//

[shader("pixel")]
PSOutput PS_Main(Ecs_PSInput input)
{
	MaterialFragment material = PSM_Main(input);
	PSOutput output = (PSOutput) 0;
	
	output.Normal3_Metallic1 = float4(material.Normal.xyz, material.Metallic);
	output.BaseColor3_Roughness1 = float4(material.BaseColor, material.Roughness);
	output.Emissive3_AO1 = float4(material.Emissive.xyz, material.Emissive.a / 256.f);
	
	return output;
}
