#include "Structs/Transform.hlsli"
#include "Structs/RenderInstance.hlsli"
#include "Structs/CameraFrameData.hlsli"

#include "Structs/Inputs/StdVertexInput.hlsli"
#include "Structs/Outputs/StdVertexOutput.hlsli"

cbuffer FrameDataBuffer
{
	CameraFrameData FrameData;
};

StructuredBuffer<Transform> Transforms;
StructuredBuffer<RenderInstance> RenderInstances;

void main(in vs_input vsIn, out vs_output_pos_only vsOut)
{
#ifdef _HAS_PREMAIN_DEPTH_PREPASS
		if (pre_main_depth_prepass(vsIn, vsOut) == true)
		{
			return;
		}
#endif

	RenderInstance instance = RenderInstances[vsIn.instance_id];

	float4x4 world = Transforms[instance.TransformId].World;
	float4 position = mul(float4(vsIn.position, 1.0), world);

	vsOut.screen_position = mul(FrameData.ViewProjection, position);

#ifdef _HAS_POSTMAIN_DEPTH_PREPASS
		post_main_depth_prepass(vsIn, vsOut);
#endif
}