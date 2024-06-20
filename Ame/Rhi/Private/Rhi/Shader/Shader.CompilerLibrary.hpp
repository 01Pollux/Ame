#pragma once

#include <map>

#include <Rhi/Device/Device.hpp>

#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Shader/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    class ShaderCompilerLibrary
    {
        template<typename Ty>
        using ComPtr = ShaderUtil::CComPtr<Ty>;

    public:
        ShaderCompilerLibrary(
            const ShaderResolveDesc& resolver,
            const ShaderBuildDesc&   desc);

        [[nodiscard]] static ShaderCompilerLibrary SpirvCompileLinkWorkaround(
            const ShaderResolveDesc& resolver,
            const ShaderLinkDesc&    desc);

        [[nodiscard]] ShaderBytecode GetBytecode() const;

    private:
        void LoadDxc();

        void LoadSourceCode(
            StringView dhaderSource);

        void Compile(
            Asset::Storage* assetStorage);

        void Validate(
            nri::GraphicsAPI       api,
            const ShaderBuildDesc& desc);

    private:
        ComPtr<IDxcValidator>      m_Validator;
        ComPtr<IDxcUtils>          m_Utils;
        ComPtr<IDxcCompiler3>      m_Compiler;
        ComPtr<IDxcIncludeHandler> m_DefaultIncludeHandler;

        WideStringView      m_EntryPoint;
        CompileShaderOption m_CompileOptions;
        ShaderType          m_ShaderStage;

        ComPtr<IDxcBlobEncoding> m_SourceCodeBlob;
        ComPtr<IDxcBlob>         m_CompiledBlob;
    };
} // namespace Ame::Rhi