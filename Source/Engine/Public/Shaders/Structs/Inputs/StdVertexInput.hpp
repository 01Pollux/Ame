#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API StructStdVertexInputShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode =
#include "StdVertexInput.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Inputs/StdVertexInput.hlsli";

        StructStdVertexInputShader()
        {
            Setup({ "S_StdVertexInput", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi