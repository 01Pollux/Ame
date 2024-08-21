#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructForwardPlusPixelOutputShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode =
#include "ForwardPlus_PixelOutput.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Outputs/ForwardPlus_PixelOutput.hlsli";

        StructForwardPlusPixelOutputShader()
        {
            Setup({ "S_Fwd+PixelOutput", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi