#include <Rhi/Shader/Shader.Linker.hpp>
#include <Rhi/Shader/Shader.Compiler.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    ShaderLinkerLibrary::ShaderLinkerLibrary(
        Device&                         rhiDevice,
        Rhi::GraphicsAPI                api,
        const ShaderCompileDesc&        desc,
        std::span<const ShaderBytecode> shaders) :
        m_CompileOptions(rhiDevice, desc),
        m_ShaderStage(desc.GetStage())
    {
        LoadDxc();
        LoadBlobs(shaders);
        RegisterLibraries();
        Link();
        Validate(api, desc);
    }

    ShaderBytecode ShaderLinkerLibrary::GetBytecode() const
    {
        if (!m_CompiledBlob)
        {
            return {};
        }

        uint8_t* shaderCode = static_cast<uint8_t*>(m_CompiledBlob->GetBufferPointer());

        size_t codeSize   = m_CompiledBlob->GetBufferSize();
        auto   outCodePtr = std::make_unique<uint8_t[]>(codeSize);

        std::copy(shaderCode, shaderCode + codeSize, outCodePtr.get());
        return ShaderBytecode(outCodePtr.release(), codeSize, m_ShaderStage);
    }

    //

    void ShaderLinkerLibrary::LoadDxc()
    {
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcValidator, IID_PPV_ARGS(&m_Validator)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_Utils)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcLinker, IID_PPV_ARGS(&m_Linker)));
    }

    //

    void ShaderLinkerLibrary::LoadBlobs(
        std::span<const ShaderBytecode> shaders)
    {
        m_Blobs =
            shaders |
            std::views::transform(
                [this](const ShaderBytecode& shader)
                {
                    CComPtr<IDxcBlobEncoding> blob;
                    ShaderUtil::ThrowShaderException(m_Utils->CreateBlobFromPinned(
                        shader.GetBytecode(),
                        static_cast<uint32_t>(shader.GetSize()),
                        DXC_CP_ACP,
                        &blob));
                    return blob;
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
        CComPtr<IDxcOperationResult> result;

        ShaderUtil::ThrowShaderException(
            m_Linker->Link(
                m_CompileOptions.EntryPoint,
                m_CompileOptions.TargetProfile.c_str(),
                m_LibrariesStr.data(),
                static_cast<uint32_t>(m_LibrariesStr.size()),
                m_CompileOptions.FinalOptions.data(),
                static_cast<uint32_t>(m_CompileOptions.FinalOptions.size()),
                &result));

        HRESULT status;
        if (FAILED(result->GetStatus(&status)))
        {
            Log::Rhi().Error("Failed to get the status of the compilation.");
            return;
        }

        if (FAILED(status))
        {
            CComPtr<IDxcBlobEncoding> error;
            if (FAILED(result->GetErrorBuffer(&error)))
            {
                Log::Rhi().Error("Failed to get the error buffer of the compilation.");
                return;
            }

            if (error)
            {
                Log::Rhi().Error(StringView(static_cast<const char*>(error->GetBufferPointer()), error->GetBufferSize()));
            }
            return;
        }

        ShaderUtil::ThrowShaderException(result->GetResult(&m_CompiledBlob));
    }

    //

    void ShaderLinkerLibrary::Validate(
        GraphicsAPI              api,
        const ShaderCompileDesc& desc)
    {
        using namespace EnumBitOperators;

        if (api != GraphicsAPI::DirectX12)
        {
            return;
        }

        if (!m_CompiledBlob)
        {
            return;
        }

        if (!desc.ShouldValidate())
        {
            return;
        }

        CComPtr<IDxcOperationResult> result;
        ShaderUtil::ThrowShaderException(m_Validator->Validate(
            ShaderUtil::GetComPtr(m_CompiledBlob),
            DxcValidatorFlags_InPlaceEdit,
            &result));

        HRESULT status;
        ShaderUtil::ThrowShaderException(result->GetStatus(&status));

        if (FAILED(status))
        {
            CComPtr<IDxcBlobEncoding> error;
            CComPtr<IDxcBlobUtf8>     errorUtf8;

            result->GetErrorBuffer(&error);
            if (error)
            {
                m_Utils->GetBlobAsUtf8(ShaderUtil::GetComPtr(error), &errorUtf8);
            }
            else
            {
                Log::Rhi().Error("Failed to get the error buffer.");
            }

            if (errorUtf8)
            {
                Log::Rhi().Error(StringView(errorUtf8->GetStringPointer(), errorUtf8->GetBufferSize()));
            }
            else
            {
                Log::Rhi().Error("Failed to get the error buffer.");
            }
            return;
        }
        else
        {
            m_CompiledBlob = nullptr;
            ShaderUtil::ThrowShaderException(result->GetResult(&m_CompiledBlob));
        }
    }

    //

    Co::result<ShaderBytecode> ShaderCompiler::LinkAsync(
        Co::executor_tag,
        Co::executor&                   executor,
        Device&                         rhiDevice,
        const ShaderCompileDesc&        desc,
        std::span<const ShaderBytecode> shaders,
        Asset::Storage*                 assetStorage)
    {
        co_return Link(rhiDevice, desc, shaders, assetStorage);
    }

    ShaderBytecode ShaderCompiler::Link(
        Device&                         rhiDevice,
        const ShaderCompileDesc&        desc,
        std::span<const ShaderBytecode> shaders,
        Asset::Storage*                 assetStorage)
    {
        auto api = rhiDevice.GetGraphicsAPI();

        if (api == Rhi::GraphicsAPI::Vulkan)
        {
            auto library = ShaderCompilerLibrary::SpirvWorkaround(rhiDevice, shaders, desc, assetStorage);
            return library.GetBytecode();
        }
        else
        {
            ShaderLinkerLibrary library(rhiDevice, api, desc, shaders);
            return library.GetBytecode();
        }
    }
} // namespace Ame::Rhi