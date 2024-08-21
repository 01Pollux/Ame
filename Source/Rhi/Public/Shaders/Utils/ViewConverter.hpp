#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class UtilsViewConverterShader : public EngineShader
    {
    private:
        static constexpr const char c_SourceCode[] =
#include "ViewConverter.hlsli.generated.hpp"
            ;

    public:
        static constexpr const char* Name = "Utils/ViewConverter.hlsli";

        UtilsViewConverterShader()
        {
            Setup({ "U_ViewConverter", Dg::SHADER_TYPE_UNKNOWN }, c_SourceCode);
        }
    };
} // namespace Ame::Rhi