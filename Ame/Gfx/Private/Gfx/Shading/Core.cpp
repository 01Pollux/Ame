#include <Gfx/Shading/Core.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::Shading
{
    const Rhi::ShaderBytecode& MaterialPipelineState::FindShader(
        Rhi::ShaderType type) const
    {
        for (const auto& shader : Shaders)
        {
            if (shader.GetStage() == type)
            {
                return shader;
            }
        }

        Log::Gfx().Fatal("Shader not found");
        std::unreachable();
    }
} // namespace Ame::Gfx::Shading