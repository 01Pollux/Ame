#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class StructStdSamplersShader : public EngineShader
    {
    private:
        static constexpr const char* c_SourceCode = R"(
#ifndef STRUCT_STD_SAMPLER_DATA
#define STRUCT_STD_SAMPLER_DATA
SamplerState Sampler_LinearClamp;
SamplerState Sampler_PointClamp;
SamplerState Sampler_LinearMirror;
SamplerState Sampler_PointWrap;
SamplerState Sampler_LinearWrap;
SamplerState Sampler_Aniso2xClamp;
SamplerState Sampler_Aniso4xClamp;
SamplerState Sampler_Aniso2xWrap;
SamplerState Sampler_Aniso4xWrap;
#endif
)";

    public:
        static constexpr const char* Name = "Structs/Rendering/StdSamplers.hlsli";

        StructStdSamplersShader()
        {
            Setup({ "S_StdMaterialFragment", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi