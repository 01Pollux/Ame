#include "Structs/Transform.hlsli"
#include "Structs/RenderInstance.hlsli"
#include "Structs/CameraFrameData.hlsli"

#include "Structs/Inputs/StdVertexInput.hlsli"
#include "Structs/Outputs/StdVertexOutput.hlsli"

void main(in vs_input vsIn, out vs_output_pos_only vsOut)
{
#ifdef _HAS_PREMAIN_DEPTH_PREPASS
	if (pre_main_depth_prepass(vsIn, vsOut) == true)
	{
		return;
	}
#endif

	RenderInstance instance = AllRenderInstances[vsIn.instance_id];

	float4x4 world = AllTransforms[instance.transform_id].World;
	float4 position = mul(float4(vsIn.position, 1.0), world);

	vsOut.screen_position = mul(FrameData.view_projection, position);

#ifdef _HAS_POSTMAIN_DEPTH_PREPASS
	post_main_depth_prepass(vsIn, vsOut);
#endif
}