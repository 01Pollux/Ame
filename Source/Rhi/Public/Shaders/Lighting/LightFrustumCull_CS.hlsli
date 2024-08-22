#include "Structs/Transform.hlsli"
#include "Structs/Light.hlsli"
#include "Structs/RenderInstance.hlsli"
#include "Structs/CameraFrameData.hlsli"

#include "Structs/Geometry/Plane.hlsli"
#include "Structs/Geometry/Frustum.hlsli"
#include "Structs/Geometry/Sphere.hlsli"

#include "Structs/Geometry/Operations.hlsli"
#include "Utils/ViewConverter.hlsli"

struct cs_input
{
	uint3 gid : SV_GroupID;
	uint3 gtid : SV_GroupThreadID;
	uint3 dtid : SV_DispatchThreadID;
	uint gi : SV_GroupIndex;
};

cbuffer FrameDataBuffer
{
	CameraFrameData FrameData;
};

StructuredBuffer<Transform> Transforms;
StructuredBuffer<Light> LightInstances;
StructuredBuffer<uint> LightIndices;

Texture2D<float> DepthTexture;

// index 0 is the count of lights
RWStructuredBuffer<uint> LightIndices_Transparent;
RWStructuredBuffer<uint> LightIndices_Opaque;

RWTexture2D<uint2> LightTexture_Transparent;
RWTexture2D<uint2> LightTexture_Opaque;

//

groupshared uint _uMinDepth;
groupshared uint _uMaxDepth;

groupshared uint _LightIndices_Transparent[MAX_LIGHT_CHUNK_SIZE];
groupshared uint _LightIndices_Opaque[MAX_LIGHT_CHUNK_SIZE];

groupshared uint _LightCount_Transparent;
groupshared uint _LightCount_Opaque;

groupshared uint _LightIndexStartOffset_Transparent;
groupshared uint _LightIndexStartOffset_Opaque;

groupshared Frustum _Frustum;

//

void compute_frustum_for_region(int2 dispatch_pos);

void append_light_transparent(uint light_index);
void append_light_opaque(uint light_index);

//

[numthreads(DISPATCH_CHUNK_SIZE, DISPATCH_CHUNK_SIZE, 1)]
void main(in cs_input csIn)
{
	uint light_count = LightIndices[0];

	int2 tex_coord = csIn.dtid.xy;
	float depth = DepthTexture.Load(int3(tex_coord, 0));
	uint udepth = asuint(depth);
	
    // initialize group shared memory
	if (csIn.gi == 0)
	{
		_uMinDepth = 0xffffffff;
		_uMaxDepth = 0;
		_LightCount_Transparent = 0;
		_LightCount_Opaque = 0;
		compute_frustum_for_region(csIn.dtid.xy);
	}

	GroupMemoryBarrierWithGroupSync();

	// find min and max depth
	InterlockedMin(_uMinDepth, udepth);
	InterlockedMax(_uMaxDepth, udepth);

	GroupMemoryBarrierWithGroupSync();
    
	float max_depth_f = asfloat(_uMaxDepth);
	float min_depth_f = asfloat(_uMinDepth);

	float max_depth = Utils_ScreenToView(float4(0, 0, max_depth_f, 1), FrameData.Viewport, FrameData.ProjectionInverse).z;
	float min_depth = Utils_ScreenToView(float4(0, 0, min_depth_f, 1), FrameData.Viewport, FrameData.ProjectionInverse).z;
	float near = Utils_ScreenToView(float4(0, 0, FrameData.NearPlane, 1), FrameData.Viewport, FrameData.ProjectionInverse).z;

	Plane min_plane = { float3(0, 0, -1.f), -min_depth };
	uint i;
	for (i = csIn.gi; i < light_count; i += DISPATCH_CHUNK_SIZE * DISPATCH_CHUNK_SIZE)
	{
		Light light = LightInstances[LightIndices[i + 1]];
		switch (light_get_type(light))
		{
        /*case LIGHT_TYPE_POINT:
		{
            Transform transform = Transforms[light.transform_id];
            float3 light_position = transform_get_position(transform);

            Sphere sphere = { light_position, light.Radius };
			if (Geometry_SphereContainsFrustum(sphere, _Frustum) == CONTAINMENT_TYPE_CONTAINS)
			{
			    append_light_transparent(LightIndices[i]);
				if (Geometry_SphereContainsPlane(sphere, min_plane) != CONTAINMENT_TYPE_CONTAINS)
				{
					append_light_opaque(LightIndices[i]);
				}
			}
			break;
		}*/
		/*case LIGHT_TYPE_SPOT:
		{
            Transform transform = Transforms[light.transform_id];
            float3 light_position = transform_get_position(transform);
            float3 light_forward = transform_get_forward(transform);

            float radius = tan(light.Angle * 0.5f) * light.Range;
            Cone cone = Geometry_ComputeCone(light_position, radius);
			if (Geometry_ConeContainsFrustum(cone, _Frustum) == CONTAINMENT_TYPE_CONTAINS)
			{
			    append_light_transparent(LightIndices[i]);
				if (Geometry_ConeContainsPlane(cone, min_plane) != CONTAINMENT_TYPE_CONTAINS)
				{
					append_light_opaque(LightIndices[i]);
				}
			}
			break;
		}*/
			case LIGHT_TYPE_DIRECTIONAL:
        {
					append_light_transparent(LightIndices[i]);
					append_light_opaque(LightIndices[i]);
					break;
				}
		}
	}

	GroupMemoryBarrierWithGroupSync();

	if (csIn.gi == 0)
	{
		InterlockedAdd(LightIndices_Opaque[0].x, _LightCount_Opaque, _LightIndexStartOffset_Opaque);
		InterlockedAdd(LightIndices_Transparent[0], _LightCount_Transparent, _LightIndexStartOffset_Transparent);

		LightTexture_Opaque[csIn.gid.xy] = int2(_LightCount_Opaque, _LightIndexStartOffset_Opaque);
		LightTexture_Transparent[csIn.gid.xy] = int2(_LightCount_Transparent, _LightIndexStartOffset_Transparent);
	}

	GroupMemoryBarrierWithGroupSync();

	for (i = csIn.gi; i < _LightCount_Opaque; i += DISPATCH_CHUNK_SIZE * DISPATCH_CHUNK_SIZE)
	{
		LightIndices_Opaque[_LightIndexStartOffset_Opaque + i + 1] = _LightIndices_Opaque[i];
	}
	for (i = csIn.gi; i < _LightCount_Transparent; i += DISPATCH_CHUNK_SIZE * DISPATCH_CHUNK_SIZE)
	{
		LightIndices_Transparent[_LightIndexStartOffset_Transparent + i + 1] = _LightIndices_Transparent[i];
	}

	write_debug_texture(csIn.gid.xy, _LightCount_Opaque);
}

//

void compute_frustum_for_region(int2 dispatch_pos)
{
	float2 dispatch_size = float2(DISPATCH_CHUNK_SIZE, DISPATCH_CHUNK_SIZE);
	_Frustum = Geometry_ComputeFrustum(FrameData.ProjectionInverse, false);
	Geometry_ComputeFrustumSubView(_Frustum, (float2)dispatch_pos, dispatch_size);
}

void append_light_transparent(uint light_index)
{
	uint index;
	InterlockedAdd(_LightCount_Transparent, 1, index);
	if (index < MAX_LIGHT_CHUNK_SIZE)
	{
		_LightIndices_Transparent[index] = light_index;
	}
}

void append_light_opaque(uint light_index)
{
	uint index;
	InterlockedAdd(_LightCount_Opaque, 1, index);
	if (index < MAX_LIGHT_CHUNK_SIZE)
	{
		_LightIndices_Opaque[index] = light_index;
	}
}