#pragma once

#include <map>
#include <ranges>
#include <vector>

#include <Rhi/Device/Device.hpp>

#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Shader/Shader.Compiler.hpp>

namespace Ame::Rhi
{
    class ShaderLinkerLibrary
    {
        template<typename Ty>
        using ComPtr = ShaderUtil::CComPtr<Ty>;

    public:
        ShaderLinkerLibrary(
            const ShaderResolveDesc& resolver,
            const ShaderLinkDesc&    desc);

        [[nodiscard]] ShaderBytecode GetBytecode() const;

    private:
        void LoadDxc();

        void LoadBlobs(
            std::span<const ShaderBytecode> shaders);

        void RegisterLibraries();

        void Link();

        void Validate(
            nri::GraphicsAPI         graphicsApi,
            const ShaderCompileDesc& desc);

    private:
        ComPtr<IDxcValidator> m_Validator;
        ComPtr<IDxcUtils>     m_Utils;
        ComPtr<IDxcLinker>    m_Linker;

        WideStringView      m_EntryPoint;
        CompileShaderOption m_CompileOptions;
        ShaderType          m_ShaderStage;

        std::vector<ComPtr<IDxcBlobEncoding>> m_Blobs;

        std::vector<WideString>      m_Libraries;
        std::vector<const WideChar*> m_LibrariesStr;

        ComPtr<IDxcBlob> m_CompiledBlob;
    };
} // namespace Ame::Rhi