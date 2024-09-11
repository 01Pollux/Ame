#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructRenderInstanceShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "RenderInstance.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/RenderInstance.hlsli";

        StructRenderInstanceShader()
        {
            Setup({ "S_RenderInstance", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi