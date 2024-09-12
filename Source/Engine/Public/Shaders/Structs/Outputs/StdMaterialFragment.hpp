#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API StructStdMaterialFragmentShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode =
#include "StdMaterialFragment.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Outputs/StdMaterialFragment.hlsli";

        StructStdMaterialFragmentShader()
        {
            Setup({ "S_StdMaterialFragment", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi