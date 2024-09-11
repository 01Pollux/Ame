#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class EmptyShader : public EngineShader
    {
    public:
        EmptyShader()
        {
            Setup({ "Empty Shader", Dg::SHADER_TYPE_UNKNOWN }, "");
        }
    };
} // namespace Ame::Rhi