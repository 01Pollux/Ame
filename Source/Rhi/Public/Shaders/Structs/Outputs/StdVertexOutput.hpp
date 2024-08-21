#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructStdVertexOutputShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode =
#include "StdVertexOutput.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Outputs/StdVertexOutput.hlsli";

        StructStdVertexOutputShader()
        {
            Setup({ "S_StdVertexOutput", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi