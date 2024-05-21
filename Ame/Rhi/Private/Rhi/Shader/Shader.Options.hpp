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
            Device&                  RhiDevice,
            const ShaderCompileDesc& Desc);

    private:
        WideString DefineMacro;

        std::vector<WideString> RegisterShift;
        std::vector<WideString> MacrosCombined;
    };
} // namespace Ame::Rhi