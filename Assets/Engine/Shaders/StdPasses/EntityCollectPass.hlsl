
#include "../Core/FrameData.hlsli"
#include "../Core/RenderInstance.hlsli"
#include "../Geometry/Transform.hlsli"

#define BLOCK_SIZE 64

struct DispatchDesc
{
	uint DrawOffset;
	uint DrawCount;
	uint CommandOffset;
	uint CounterOffset;
};

//

AME_RESOURCE(ConstantBuffer<FrameDesc>, FrameInfo, b, 0, 1);

AME_RESOURCE(StructuredBuffer<Transform>, Transforms, t, 0, 2);
AME_RESOURCE(StructuredBuffer<RenderInstance>, RenderInstances, t, 1, 2);

AME_RESOURCE(RWBuffer<uint>, Commands, u, 0, 3);
AME_RESOURCE(RWBuffer<uint>, CommandCount, u, 1, 3);

AME_PUSH_CONSTANT(DispatchDesc, DispatchData, 0);

//

groupshared uint s_DrawCount;

//

[numthreads(BLOCK_SIZE, 1, 1)]
void CS_Main(uint threadId : SV_DispatchThreadId)
{
	DrawHelper drawHelper;
	drawHelper.Initialize(Commands, CommandCount);
	
	if (threadId == 0)
		s_DrawCount = 0;

	GroupMemoryBarrierWithGroupSync();

	for (uint i = threadId; i < DispatchData.DrawCount; i += BLOCK_SIZE)
	{
		uint index = DispatchData.DrawOffset + i;
		RenderInstance instance = RenderInstances[index];
		
		uint commandIndex;
		InterlockedAdd(s_DrawCount, 1, commandIndex);
		commandIndex += DispatchData.CommandOffset;
		drawHelper.FillDrawIndexedDesc(commandIndex, instance.IndexCount, 1, instance.IndexOffset, instance.VertexOffset, 0);
	}
	
	GroupMemoryBarrierWithGroupSync();

	if (threadId == 0)
		CommandCount[DispatchData.CounterOffset] = s_DrawCount;
}
