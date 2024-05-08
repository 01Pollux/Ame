#ifndef AME_CORE_H
#define AME_CORE_H


// Indirect commands filling // TODO: change to StructuredBuffers?
#ifdef AME_USE_BYTE_ADDRESS_BUFFER
#define AME_BUFFER_WRITE(buffer, offset, index, value) buffer.Store(offset * 4 + index * 4, value)
#else
#define AME_BUFFER_WRITE(buffer, offset, index, value) buffer[offset + index] = value
#endif

// "DrawDesc"
#define AME_FILL_DRAW_DESC(buffer, cmdIndex, vertexNum, instanceNum, baseVertex, baseInstance) \
    AME_BUFFER_WRITE(buffer, cmdIndex * 4, 0, vertexNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 4, 1, instanceNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 4, 2, baseVertex); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 4, 3, baseInstance)

// see "DrawIndexedDesc"
#define AME_FILL_DRAW_INDEXED_DESC(buffer, cmdIndex, indexNum, instanceNum, baseIndex, baseVertex, baseInstance) \
    AME_BUFFER_WRITE(buffer, cmdIndex * 5, 0, indexNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 5, 1, instanceNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 5, 2, baseIndex); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 5, 3, baseVertex); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 5, 4, baseInstance)

// see "DrawBaseDesc"
#define AME_FILL_DRAW_BASE_DESC(buffer, cmdIndex, vertexNum, instanceNum, baseVertex, baseInstance) \
    AME_BUFFER_WRITE(buffer, cmdIndex * 6, 0, baseVertex);  \
    AME_BUFFER_WRITE(buffer, cmdIndex * 6, 1, baseInstance);  \
    AME_BUFFER_WRITE(buffer, cmdIndex * 6, 2, vertexNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 6, 3, instanceNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 6, 4, baseVertex); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 6, 5, baseInstance)

// see "DrawIndexedBaseDesc"
#define AME_FILL_DRAW_INDEXED_BASE_DESC(buffer, cmdIndex, indexNum, instanceNum, baseIndex, baseVertex, baseInstance) \
    AME_BUFFER_WRITE(buffer, cmdIndex * 7, 0, baseVertex); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 7, 1, baseInstance); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 7, 2, indexNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 7, 3, instanceNum); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 7, 4, baseIndex); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 7, 5, baseVertex); \
    AME_BUFFER_WRITE(buffer, cmdIndex * 7, 6, baseInstance)


#ifdef AME_SHADER_COMPILER_SPIRV

#define AME_RESOURCE(type, name, regType, bindingIndex, setIndex) \
		type name : register(regType##bindingIndex, space##setIndex)

#define AME_PUSH_CONSTANT(type, name, bindingIndex)
		[[vk::push_constant]] type name

    // Draw parameters (full support, requires SPV_KHR_shader_draw_parameters)
#define AME_ENABLE_DRAW_PARAMETERS

#define AME_DECLARE_DRAW_PARAMETERS \
        int AME_VERTEX_ID_OFFSET : SV_VertexID, \
        uint AME_INSTANCE_ID_OFFSET : SV_InstanceID, \
        [[vk::builtin("BaseVertex")]] int AME_BASE_VERTEX : _SV_Nothing, \
        [[vk::builtin("BaseInstance")]] uint AME_BASE_INSTANCE : _SV_Nothing

#define AME_VERTEX_ID (AME_VERTEX_ID_OFFSET - AME_BASE_VERTEX)
#define AME_INSTANCE_ID (AME_INSTANCE_ID_OFFSET - AME_BASE_INSTANCE)


#else // AME_SHADER_COMPILER_SPIRV


#define AME_RESOURCE(type, name, regType, bindingIndex, setIndex) \
		type name : register( regType##bindingIndex, space##setIndex )

#define AME_PUSH_CONSTANT(type, name, bindingIndex) \
		ConstantBuffer<type> name : register(b##bindingIndex, space0)

    // Draw parameters
#if (AME_SHADER_MODEL < 68)
#ifdef AME_ENABLE_DRAW_PARAMETERS_EMULATION
            // Full support (emulation)
#define AME_ENABLE_DRAW_PARAMETERS \
                struct _BaseAttributeConstants { \
                    int baseVertex; \
                    uint baseInstance; \
                }; \
                ConstantBuffer<_BaseAttributeConstants> _BaseAttributes : register(b0, space999) // see BASE_ATTRIBUTES_EMULATION_SPACE

#define AME_DECLARE_DRAW_PARAMETERS \
                uint AME_VERTEX_ID : SV_VertexID, \
                uint AME_INSTANCE_ID : SV_InstanceID

#define AME_BASE_VERTEX _BaseAttributes.baseVertex
#define AME_BASE_INSTANCE _BaseAttributes.baseInstance
#define AME_VERTEX_ID_OFFSET (AME_BASE_VERTEX + AME_VERTEX_ID)
#define AME_INSTANCE_ID_OFFSET (AME_BASE_INSTANCE + AME_INSTANCE_ID)

#undef AME_FILL_DRAW_DESC
#define AME_FILL_DRAW_DESC AME_FILL_DRAW_BASE_DESC

#undef AME_FILL_DRAW_INDEXED_DESC
#define AME_FILL_DRAW_INDEXED_DESC AME_FILL_DRAW_INDEXED_BASE_DESC
#else // AME_ENABLE_DRAW_PARAMETERS_EMULATION
            // Partial support
#define AME_ENABLE_DRAW_PARAMETERS

#define AME_DECLARE_DRAW_PARAMETERS \
                uint AME_VERTEX_ID : SV_VertexID, \
                uint AME_INSTANCE_ID : SV_InstanceID

#define AME_BASE_VERTEX AME_BASE_VERTEX_is_unsupported
#define AME_BASE_INSTANCE AME_BASE_INSTANCE_is_unsupported
#define AME_VERTEX_ID_OFFSET AME_VERTEX_ID_OFFSET_is_unsupported
#define AME_INSTANCE_ID_OFFSET AME_INSTANCE_ID_OFFSET_is_unsupported
#endif // AME_ENABLE_DRAW_PARAMETERS_EMULATION
#else // AME_SHADER_MODEL >= 68
        // Full support
#define AME_ENABLE_DRAW_PARAMETERS

#define AME_DECLARE_DRAW_PARAMETERS \
            uint AME_VERTEX_ID : SV_VertexID, \
            uint AME_INSTANCE_ID : SV_InstanceID, \
            int AME_BASE_VERTEX : SV_StartVertexLocation, \
            uint AME_BASE_INSTANCE : SV_StartInstanceLocation
#endif // AME_SHADER_MODEL

#endif // AME_SHADER_COMPILER_SPIRV


class DrawHelper
{
	RWBuffer<uint> CommandBuffer;
    RWBuffer<uint> CommandCount;
    
    void Initialize(RWBuffer<uint> commandBuffer, RWBuffer<uint> commandCount)
	{
		CommandBuffer = commandBuffer;
		CommandCount = commandCount;
	}
    
    void FillDrawDesc(uint cmdIndex, uint vertexNum, uint instanceNum, uint baseVertex, uint baseInstance)
	{
		AME_FILL_DRAW_DESC(CommandBuffer, cmdIndex, vertexNum, instanceNum, baseVertex, baseInstance);
	}
    
    void FillDrawIndexedDesc(uint cmdIndex, uint indexNum, uint instanceNum, uint baseIndex, uint baseVertex, uint baseInstance)
	{
		AME_FILL_DRAW_INDEXED_DESC(CommandBuffer, cmdIndex, indexNum, instanceNum, baseIndex, baseVertex, baseInstance);
	}
};

#endif // AME_CORE_SHADER_H