
#include <Rhi/Shader/Shader.Common.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::ShaderUtil
{
    void ThrowShaderException(
        HRESULT Result)
    {
        if (FAILED(Result))
        {
            Log::Rhi().Fatal("Failed to compile shader: {:X}", Result);
        }
    }
} // namespace Ame::Rhi::ShaderUtil