#pragma once

#include <Shaders/EngineShader.hpp>

namespace Ame::Rhi
{
    class AME_ENGINE_API EmptyShader : public EngineShader
    {
    public:
        EmptyShader()
        {
            Setup({ "Empty Shader", Dg::SHADER_TYPE_UNKNOWN }, "");
        }
    };
} // namespace Ame::Rhi