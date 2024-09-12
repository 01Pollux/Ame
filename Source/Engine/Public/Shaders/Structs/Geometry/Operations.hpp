#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API GeometryOperationsShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Operations.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Geometry/Operations.hlsli";

        GeometryOperationsShader()
        {
            Setup({ "G_Operations", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi