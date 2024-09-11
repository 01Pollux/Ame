#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class ImGuiRenderVertexShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "ImGuiRenderShader_VS.hlsli.generated.hpp"
            ;

    public:
        ImGuiRenderVertexShader()
        {
            Setup({ "R_ImGui_VS", Dg::SHADER_TYPE_VERTEX }, c_SourceCode);
        }
    };

    class ImGuiRenderPixelShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "ImGuiRenderShader_PS.hlsli.generated.hpp"
            ;

    public:
        ImGuiRenderPixelShader()
        {
            Setup({ "R_ImGui_PS", Dg::SHADER_TYPE_PIXEL, true }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi