#ifndef STRUCT_STD_VERTEX_INPUT
#define STRUCT_STD_VERTEX_INPUT
struct vs_input
{
	uint vertex_id : SV_VertexID;
	uint instance_id : SV_InstanceID;
	float3 position : ATTRIB0;
	float3 normal : ATTRIB1;
	float2 tex_coord : ATTRIB2;
	float3 tangent : ATTRIB3;
};
#endif