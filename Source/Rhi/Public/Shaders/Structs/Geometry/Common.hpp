#pragma once

#include <Shaders/EngineShader.hpp>

#include <Shaders/Structs/Transform.hpp>
#include <Shaders/Structs/RenderInstance.hpp>
#include <Shaders/Structs/CameraFrameData.hpp>

#include <Shaders/Structs/Inputs/StdVertexInput.hpp>
#include <Shaders/Structs/Inputs/StdPixelInput.hpp>

#include <Shaders/Structs/Outputs/StdVertexOutput.hpp>
#include <Shaders/Structs/Outputs/StdMaterialFragment.hpp>
#include <Shaders/Structs/Outputs/ForwardPlus_PixelOutput.hpp>

namespace Ame::Rhi
{
    class GeometryPlanesShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] = R"(
#ifndef AME_GEOMETRY_COMMON
#define AME_GEOMETRY_COMMON
#define CONTAINMENT_TYPE_CONTAINS 0
#define CONTAINMENT_TYPE_INTERSECTS 1
#define CONTAINMENT_TYPE_DISJOINT 2
#endif
)";

    public:
        static constexpr const char* Name = "Structs/Geometry/Common.hlsli";

        GeometryPlanesShader()
        {
            Setup({ "G_Common", Dg::SHADER_TYPE_UNKNOWN}, c_SourceCode);
        }
    };
} // namespace Ame::Rhi