#pragma once

#include <vector>

#include <Core/String.hpp>
#include <Rhi/Shader/Shader.Common.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    struct CompileShaderOption
    {
        WideString                   TargetProfile;
        const WideChar*              EntryPoint = nullptr;
        std::vector<const WideChar*> FinalOptions;
        GraphicsAPI                  Api;

        CompileShaderOption(
            Device&                  rhiDevice,
            const ShaderCompileDesc& desc);

    private:
        WideString m_DefineMacro;

        std::vector<WideString> m_RegisterShift;
        std::vector<WideString> m_MacrosCombined;
    };
} // namespace Ame::Rhi