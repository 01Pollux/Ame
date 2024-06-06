#ifndef AME_CORE_INSTANCEDATA_H
#define AME_CORE_INSTANCEDATA_H

#include "Ame.hlsli"

struct RenderInstance
{
	uint AABBId;
	uint TransformId;
	
	uint VertexOffset;
	uint VertexSize;
	uint IndexOffset;
	uint IndexCount;
};

#endif