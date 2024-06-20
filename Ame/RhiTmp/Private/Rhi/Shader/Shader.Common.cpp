
#include <Rhi/Shader/Shader.Common.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::ShaderUtil
{
    void ThrowShaderException(
        HRESULT hr)
    {
        if (FAILED(hr))
        {
            Log::Rhi().Fatal("Failed to compile shader: {:X}", hr);
        }
    }
} // namespace Ame::Rhi::ShaderUtil