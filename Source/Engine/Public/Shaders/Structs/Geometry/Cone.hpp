#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class GeometryPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Cone.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Geometry/Planes.hlsli";

        GeometryPlanesShader()
        {
            Setup({ "G_Planes", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi