
#include "../Core/Material.hlsli"

//

[shader("pixel")]
float4 PS_Main(Ecs_PSInput input) : SV_Target
{
	MaterialFragment material = PSM_Main(input);
	return float4(material.BaseColor, material.Roughness);
}
