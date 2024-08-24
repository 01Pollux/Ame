#include "Structs/Transform.hlsli"
#include "Structs/Light.hlsli"
#include "Structs/CameraFrameData.hlsli"

#include "Structs/Inputs/StdPixelInput.hlsli"
#include "Structs/Outputs/StdMaterialFragment.hlsli"
#include "Structs/Outputs/ForwardPlus_PixelOutput.hlsli"

#include "Structs/Lighting/LightingResult.hlsli"
#include "Structs/Lighting/Operations.hlsli"

void main(in ps_input psIn, out ps_output psOut)
{
	material_fragment fragment;

#ifdef _HAS_PREMAIN
	psOut.color = (float4)0;
	if (pre_main(psIn, fragment) == false)
	{
		return;
	}
	
	float4 screen_pos = psIn.screen_position;
	float2 uv = screen_pos.xy / screen_pos.w;
	
	LightingSurface light_surface;
	light_surface.position = psIn.world_position;
	light_surface.normal = fragment.normal;
	light_surface.specular_power = fragment.specular;
	light_surface.view_dir = normalize(FrameData_GetPosition(FrameData) - psIn.world_position);

	LightingResult lighting = (LightingResult) 0;
	LightingResult_ComputeFromLinkedList(lighting, uv, light_surface);

	float3 color = lighting.diffuse.rgb + lighting.specular.rgb;
	psOut.color = float4(color, 1.0) * fragment.ao;

#else
	psOut.color = float4(1.0, 1.0, 1.0, 1.0);
#endif
}