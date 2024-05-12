#pragma once

#include <vector>

#include <Core/String.hpp>
#include <Rhi/Shader/Shader.Common.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    struct CompileShaderOption
    {
        std::vector<const wchar_t*> FinalOptions;
        GraphicsAPI                 Api;

        CompileShaderOption(
            Device&                  RhiDevice,
            const ShaderCompileDesc& Desc);

    private:
        WideString Model;
        WideString DefineMacro;

        std::vector<WideString> RegisterShift;
        std::vector<WideString> MacrosCombined;
    };
} // namespace Ame::Rhi