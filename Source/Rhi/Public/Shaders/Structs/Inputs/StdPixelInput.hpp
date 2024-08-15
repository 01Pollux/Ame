#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructStdPixelInputShader : public EngineShader
    {
    private:
        // Must be identical to the struct definition in VertexInput.hpp.
        static constexpr const char* c_SourceCode = R"(
	#ifndef STRUCT_STD_PIXEL_INPUT
	#define STRUCT_STD_PIXEL_INPUT
    struct ps_input{float4 screen_position:SV_POSITION;float3 world_position:WORLD_POSITION;float3 world_normal:WORLD_NORMAL;float2 tex_coord:TEXCOORD;float3 world_tangent:WORLD_TANGENT;};
	#endif
)";

    public:
        static constexpr const char* Name = "Structs/Inputs/StdPixelInput.hlsli";

        StructStdPixelInputShader()
        {
            Setup({ "S_StdPixelInput", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi