#include <Rhi/Shader/Shader.Linker.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    ShaderLinkerLibrary::ShaderLinkerLibrary(
        Device&                         RhiDevice,
        const ShaderCompileDesc&        Desc,
        std::span<const ShaderBytecode> Shaders) :
        m_CompileOptions(RhiDevice, Desc),
        m_ShaderStage(Desc.GetStage())
    {
        LoadDxc();
        LoadBlobs(Shaders);
        RegisterLibraries();
        Link();
    }

    ShaderBytecode ShaderLinkerLibrary::GetBytecode() const
    {
        if (!m_CompiledBlob)
        {
            return {};
        }

        uint8_t* CompiledShaderCode = static_cast<uint8_t*>(m_CompiledBlob->GetBufferPointer());

        size_t CodeSize = m_CompiledBlob->GetBufferSize();
        auto   CodePtr  = std::make_unique<uint8_t[]>(CodeSize);

        std::copy(CompiledShaderCode, CompiledShaderCode + CodeSize, CodePtr.get());
        return ShaderBytecode(CodePtr.release(), CodeSize, m_ShaderStage);
    }

    //

    void ShaderLinkerLibrary::LoadDxc()
    {
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_Utils)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcLinker, IID_PPV_ARGS(&m_Linker)));
    }

    //

    void ShaderLinkerLibrary::LoadBlobs(
        std::span<const ShaderBytecode> Shaders)
    {
        m_Blobs =
            Shaders |
            std::views::transform(
                [this](const ShaderBytecode& Shader)
                {
                    CComPtr<IDxcBlobEncoding> Blob;
                    ShaderUtil::ThrowShaderException(m_Utils->CreateBlobFromPinned(
                        Shader.GetBytecode(),
                        static_cast<uint32_t>(Shader.GetSize()),
                        DXC_CP_ACP,
                        &Blob));
                    return Blob;
                }) |
            std::ranges::to<std::vector>();
    }

    void ShaderLinkerLibrary::RegisterLibraries()
    {
        m_Libraries.reserve(m_Blobs.size());

        for (size_t i = 0; i < m_Blobs.size(); i++)
        {
            m_Libraries.emplace_back(std::to_wstring(i));

            ShaderUtil::ThrowShaderException(
                m_Linker->RegisterLibrary(
                    std::to_wstring(i).c_str(),
                    m_Blobs[i].Get()));
        }

        m_LibrariesStr =
            m_Libraries |
            std::views::transform(
                [](const WideString& Library)
                {
                    return Library.c_str();
                }) |
            std::ranges::to<std::vector>();
    }

    void ShaderLinkerLibrary::Link()
    {
        CComPtr<IDxcOperationResult> Result;

        ShaderUtil::ThrowShaderException(
            m_Linker->Link(
                m_CompileOptions.EntryPoint,
                m_CompileOptions.TargetProfile.c_str(),
                m_LibrariesStr.data(),
                static_cast<uint32_t>(m_LibrariesStr.size()),
                m_CompileOptions.FinalOptions.data(),
                static_cast<uint32_t>(m_CompileOptions.FinalOptions.size()),
                &Result));

        HRESULT Status;
        if (FAILED(Result->GetStatus(&Status)))
        {
            Log::Rhi().Error("Failed to get the status of the compilation.");
            return;
        }

        if (FAILED(Status))
        {
            CComPtr<IDxcBlobEncoding> ErrorBlob;
            if (FAILED(Result->GetErrorBuffer(&ErrorBlob)))
            {
                Log::Rhi().Error("Failed to get the error buffer of the compilation.");
                return;
            }

            if (ErrorBlob)
            {
                Log::Rhi().Error(StringView(static_cast<const char*>(ErrorBlob->GetBufferPointer()), ErrorBlob->GetBufferSize()));
            }
            return;
        }

        ShaderUtil::ThrowShaderException(Result->GetResult(&m_CompiledBlob));
    }

    //

    Co::result<ShaderBytecode> ShaderCompiler::LinkAsync(
        Co::executor_tag,
        Co::executor&                   Executor,
        Device&                         RhiDevice,
        const ShaderCompileDesc&        Desc,
        std::span<const ShaderBytecode> Shaders)
    {
        co_return Link(RhiDevice, Desc, Shaders);
    }

    ShaderBytecode ShaderCompiler::Link(
        Device&                         RhiDevice,
        const ShaderCompileDesc&        Desc,
        std::span<const ShaderBytecode> Shaders)
    {
        ShaderLinkerLibrary Library(RhiDevice, Desc, Shaders);
        return Library.GetBytecode();
    }
} // namespace Ame::Rhi