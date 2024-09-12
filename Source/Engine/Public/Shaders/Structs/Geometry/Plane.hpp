#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API StructGeometryPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Plane.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Geometry/Plane.hlsli";

        StructGeometryPlanesShader()
        {
            Setup({ "SG_Plane", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi