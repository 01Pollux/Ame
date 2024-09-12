#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API StructCameraFrameDataShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "CameraFrameData.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Structs/CameraFrameData.hlsli";

        StructCameraFrameDataShader()
        {
            Setup({ "S_CameraFrameData", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi