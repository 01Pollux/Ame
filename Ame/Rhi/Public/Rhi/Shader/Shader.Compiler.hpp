#pragma once

#include <Rhi/Shader/Shader.Desc.hpp>

namespace Ame::Rhi
{
    struct ShaderCompiler
    {
        /// <summary>
        /// Compiles the shader source code.
        /// </summary>
        [[nodiscard]] static ShaderBytecode Compile(
            const ShaderResolveDesc& resolver,
            const ShaderBuildDesc&   desc);

        /// <summary>
        /// Links the shader bytecodes.
        /// </summary>
        [[nodiscard]] static ShaderBytecode Link(
            const ShaderResolveDesc& resolver,
            const ShaderLinkDesc&    desc);
    };
} // namespace Ame::Rhi