#pragma once

#include <map>
#include <ranges>
#include <vector>

#include <Rhi/Device/Device.hpp>

#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Shader/Shader.Handler.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    class ShaderLinkerLibrary
    {
        template<typename Ty>
        using CComPtr = ShaderUtil::CComPtr<Ty>;

    public:
        ShaderLinkerLibrary(
            Device&                         RhiDevice,
            const ShaderCompileDesc&        Desc,
            std::span<const ShaderBytecode> Shaders);

        [[nodiscard]] ShaderBytecode GetBytecode() const;

    private:
        void LoadDxc();

        void LoadBlobs(
            std::span<const ShaderBytecode> Shaders);

        void RegisterLibraries();

        void Link();

    private:
        CComPtr<IDxcUtils>  m_Utils;
        CComPtr<IDxcLinker> m_Linker;

        CompileShaderOption m_CompileOptions;
        ShaderType          m_ShaderStage;

        std::vector<CComPtr<IDxcBlobEncoding>> m_Blobs;

        std::vector<WideString>      m_Libraries;
        std::vector<const WideChar*> m_LibrariesStr;

        CComPtr<IDxcBlob> m_CompiledBlob;
    };
} // namespace Ame::Rhi