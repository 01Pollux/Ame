#include <Gfx/Shading/Core.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    const Rhi::ShaderBytecode& MaterialPipelineState::FindShader(
        Rhi::ShaderType Type) const
    {
        for (const auto& Shader : Shaders)
        {
            if (Shader.GetStage() == Type)
            {
                return Shader;
            }
        }

        Log::Renderer().Fatal("Shader not found");
        std::unreachable();
    }
} // namespace Ame::Gfx::Shading