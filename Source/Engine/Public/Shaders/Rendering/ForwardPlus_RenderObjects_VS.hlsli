#include "Structs/Transform.hlsli"
#include "Structs/RenderInstance.hlsli"
#include "Structs/CameraFrameData.hlsli"

#include "Structs/Inputs/StdVertexInput.hlsli"
#include "Structs/Outputs/StdVertexOutput.hlsli"

void main(in vs_input vsIn, out vs_output vsOut)
{
#ifdef _HAS_PREMAIN
	if (pre_main(vsIn, vsOut) == true)
	{
		return;
	}
#endif

	RenderInstance instance = AllRenderInstances[vsIn.instance_id];

	float4x4 world = AllTransforms[instance.transform_id].World;
	float4 position = mul(float4(vsIn.position, 1.0), world);

	vsOut.screen_position = mul(FrameData.view_projection, position);
	vsOut.world_position = position.xyz;
	vsOut.world_normal = mul(float4(vsIn.normal, 0.0), world).xyz;
	vsOut.world_tangent = mul(float4(vsIn.tangent, 0.0), world).xyz;
	vsOut.tex_coord = vsIn.tex_coord;

#ifdef _HAS_POSTMAIN
	post_main(vsIn, vsOut);
#endif
}