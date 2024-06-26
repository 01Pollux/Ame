#pragma once

#include <map>

#include <Rhi/Device/Device.hpp>

#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    class ShaderCompilerLibrary
    {
        template<typename Ty>
        using CComPtr = ShaderUtil::CComPtr<Ty>;

    public:
        ShaderCompilerLibrary(
            Device&                  rhiDevice,
            StringView               shaderSource,
            const ShaderCompileDesc& desc,
            Asset::Storage*          assetStorage);

        [[nodiscard]] static ShaderCompilerLibrary SpirvCompileLinkWorkaround(
            Device&                         rhiDevice,
            std::span<const ShaderBytecode> shaderCodes,
            const ShaderCompileDesc&        desc,
            Asset::Storage*                 assetStorage);

        [[nodiscard]] ShaderBytecode GetBytecode() const;

    private:
        void LoadDxc();

        void LoadSourceCode(
            StringView dhaderSource);

        void Compile(
            Asset::Storage* assetStorage);

        void Validate(
            GraphicsAPI              api,
            const ShaderCompileDesc& desc);

    private:
        CComPtr<IDxcValidator>      m_Validator;
        CComPtr<IDxcUtils>          m_Utils;
        CComPtr<IDxcCompiler3>      m_Compiler;
        CComPtr<IDxcIncludeHandler> m_DefaultIncludeHandler;

        CompileShaderOption m_CompileOptions;
        ShaderType          m_ShaderStage;

        CComPtr<IDxcBlobEncoding> m_SourceCodeBlob;
        CComPtr<IDxcBlob>         m_CompiledBlob;
    };
} // namespace Ame::Rhi