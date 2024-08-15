#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructStdVertexInputShader : public EngineShader
    {
    private:
        // Must be identical to the struct definition in VertexInput.hpp.
        static constexpr const char* c_SourceCode = R"(
	#ifndef STRUCT_STD_VERTEX_INPUT
	#define STRUCT_STD_VERTEX_INPUT
    struct vs_input{uint vertex_id:SV_VertexID;uint instance_id:SV_InstanceID;float3 position:ATTRIB0;float3 normal:ATTRIB1;float2 tex_coord:ATTRIB2;float3 tangent:ATTRIB3;};
	#endif
)";

    public:
        static constexpr const char* Name = "Structs/Inputs/StdVertexInput.hlsli";

        StructStdVertexInputShader()
        {
            Setup({ "S_StdVertexInput", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi