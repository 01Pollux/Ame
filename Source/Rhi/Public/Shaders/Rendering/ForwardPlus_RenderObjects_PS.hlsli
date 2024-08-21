#include "Structs/Inputs/StdPixelInput.hlsli"
#include "Structs/Outputs/StdMaterialFragment.hlsli"
#include "Structs/Outputs/ForwardPlus_PixelOutput.hlsli"

void main(in ps_input psIn, out ps_output psOut)
{
	material_fragment fragment;

#ifdef _HAS_PREMAIN
	psOut.color = (float4)0;
	if (pre_main(psIn, fragment) == false)
	{
		return;
	}

	// suppose that the light is at the camera position
	float3 light_dir = normalize(float3(0.0, 0.0, 1.0) - psIn.world_position);
	float3 normal = normalize(fragment.normal);
	float3 view_dir = normalize(float3(0.0, 0.0, 1.0) - psIn.world_position);

	float3 ambient = fragment.base_color.rgb;
	float3 diffuse = fragment.base_color.rgb * max(dot(normal, light_dir), 0.0);
	float3 specular = fragment.specular.rgb * pow(max(dot(reflect(-light_dir, normal), view_dir), 0.0), fragment.shininess.r);

	psOut.color = float4(ambient + diffuse + specular, 1.0) * fragment.ao;

#else
	psOut.color = float4(1.0, 1.0, 1.0, 1.0);
#endif
}