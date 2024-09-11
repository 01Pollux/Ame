#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructGeometryFrustumPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Frustum.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Geometry/Frustum.hlsli";

        StructGeometryFrustumPlanesShader()
        {
            Setup({ "SG_FrustumPlanes", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi