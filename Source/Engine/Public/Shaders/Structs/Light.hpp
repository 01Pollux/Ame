#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API StructLightShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Light.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Light.hlsli";

        StructLightShader()
        {
            Setup({ "S_Light", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi