#pragma once

#include <map>

#include <Rhi/Device/Device.hpp>

#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Shader/Shader.Handler.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    class ShaderCompilerLibrary
    {
        template<typename Ty>
        using CComPtr = ShaderUtil::CComPtr<Ty>;

    public:
        ShaderCompilerLibrary(
            Device&                  RhiDevice,
            StringView               ShaderSource,
            const ShaderCompileDesc& Desc,
            Asset::Storage*          AssetStorage);

        [[nodiscard]] ShaderBytecode GetBytecode() const;

    private:
        void LoadDxc();

        void LoadSourceCode(
            StringView ShaderSource);

        void Compile(
            Asset::Storage* AssetStorage);

        void Validate(
            GraphicsAPI              Api,
            const ShaderCompileDesc& Desc);

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