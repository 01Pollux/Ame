#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API StructStdSamplersShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "StandardSamplers.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Rendering/StdSamplers.hlsli";

        StructStdSamplersShader()
        {
            Setup({ "S_StdMaterialFragment", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi