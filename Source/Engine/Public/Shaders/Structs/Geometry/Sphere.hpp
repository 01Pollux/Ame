#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API StructGeometrySphereShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "Sphere.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/Geometry/Sphere.hlsli";

        StructGeometrySphereShader()
        {
            Setup({ "SG_Sphere", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi