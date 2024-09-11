#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructDrawIndexedIndirectCommandShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "DrawIndexedIndirectCommand.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/DrawIndexedIndirectCommand.hlsli";

        StructDrawIndexedIndirectCommandShader()
        {
            Setup({ "Struct DrawIndexedIndirectCommand", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi