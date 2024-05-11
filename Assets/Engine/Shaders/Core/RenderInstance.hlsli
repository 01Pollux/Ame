#ifndef AME_CORE_INSTANCEDATA_H
#define AME_CORE_INSTANCEDATA_H

#include "Ame.hlsli"

struct RenderInstance
{
	uint AABBIndex;
	uint TransformIndex;
	
	uint VertexOffset;
	uint VertexSize;
	uint IndexOffset;
	uint IndexCount;
};

#endif