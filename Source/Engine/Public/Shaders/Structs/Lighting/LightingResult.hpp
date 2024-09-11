#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructLightingResultShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "LightingResult.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Lighting/LightingResult.hlsli";

        StructLightingResultShader()
        {
            Setup({ "SL_LightingResult", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi