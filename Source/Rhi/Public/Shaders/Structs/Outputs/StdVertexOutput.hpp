#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructStdVertexOutputShader : public EngineShader
    {
    private:
        // Must be identical to the struct definition in VertexInput.hpp.
        static constexpr const char* c_SourceCode = R"(
	#ifndef STRUCT_STD_VERTEX_OUTPUT
	#define STRUCT_STD_VERTEX_OUTPUT
    struct vs_output{float4 screen_position:SV_POSITION;float3 world_position:WORLD_POSITION;float3 world_normal:WORLD_NORMAL;float2 tex_coord:TEXCOORD;float3 world_tangent:WORLD_TANGENT;};
	#endif
)";

    public:
        static constexpr const char* Name = "Structs/Outputs/StdVertexOutput.hlsli";

        StructStdVertexOutputShader()
        {
            Setup({ "S_StdVertexOutput", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi