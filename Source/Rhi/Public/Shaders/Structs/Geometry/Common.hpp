#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class GeometryPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Common.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Geometry/Common.hlsli";

        GeometryPlanesShader()
        {
            Setup({ "G_Common", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi