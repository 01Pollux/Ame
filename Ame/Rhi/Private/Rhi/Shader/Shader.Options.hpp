#pragma once

#include <vector>

#include <Core/String.hpp>
#include <Rhi/Shader/Shader.Common.hpp>
#include <Rhi/Shader/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    struct CompileShaderOption
    {
        WideString                   TargetProfile;
        const WideChar*              EntryPoint = nullptr;
        std::vector<const WideChar*> FinalOptions;
        nri::GraphicsAPI             Api;

        CompileShaderOption(
            const ShaderResolveDesc& resolver,
            const ShaderCompileDesc& desc);

    private:
        std::vector<WideString> m_RegisterShift;
        std::vector<WideString> m_MacrosCombined;
    };
} // namespace Ame::Rhi