#ifndef STRUCT_DRAWINDEXEDINDIRECTCOMMAND
#define STRUCT_DRAWINDEXEDINDIRECTCOMMAND
struct DrawIndexedIndirectCommand
{
	uint NumIndices;
	uint NumInstances;
	uint FirstIndexLocation;
	uint BaseVertex;
	uint FirstInstanceLocation;
};
#define DrawIndexedIndirectCommand_Size 20
#define DrawIndexedIndirectCommand_Store(commandIndex,buffer,command,drawOffset) \
        buffer.Store(drawOffset + commandIndex * DrawIndexedIndirectCommand_Size + 0, command.NumIndices); \
        buffer.Store(drawOffset + commandIndex * DrawIndexedIndirectCommand_Size + 4, command.NumInstances); \
        buffer.Store(drawOffset + commandIndex * DrawIndexedIndirectCommand_Size + 8, command.FirstIndexLocation); \
        buffer.Store(drawOffset + commandIndex * DrawIndexedIndirectCommand_Size + 12, command.BaseVertex); \
        buffer.Store(drawOffset + commandIndex * DrawIndexedIndirectCommand_Size + 16, command.FirstInstanceLocation);
#endif