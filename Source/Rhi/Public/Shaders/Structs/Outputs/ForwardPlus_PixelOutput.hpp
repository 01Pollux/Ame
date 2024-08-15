#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructForwardPlusPixelOutputShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode = R"(
	#ifndef STRUCT_STD_PIXEL_OUTPUT
	#define STRUCT_STD_PIXEL_OUTPUT
    struct ps_output{float4 color:SV_TARGET;};
	#endif
)";

    public:
        static constexpr const char* Name = "Structs/Outputs/ForwardPlus_StdPixelOutput.hlsli";

        StructForwardPlusPixelOutputShader()
        {
            Setup({ "S_Fwd+PixelOutput", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi