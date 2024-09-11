#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructTransformShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Transform.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Transform.hlsli";

        StructTransformShader()
        {
            Setup({ "S_Transform", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi