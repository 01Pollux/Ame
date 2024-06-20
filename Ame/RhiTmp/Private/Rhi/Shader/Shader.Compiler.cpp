#include <Rhi/Shader/Shader.Compiler.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    ShaderCompilerLibrary::ShaderCompilerLibrary(
        Device&                  rhiDevice,
        StringView               shaderSource,
        const ShaderCompileDesc& desc,
        Asset::Storage*          assetStorage) :
        m_CompileOptions(rhiDevice, desc),
        m_ShaderStage(desc.GetStage())
    {
        LoadDxc();
        LoadSourceCode(shaderSource);

        auto api = rhiDevice.GetGraphicsAPI();
        // As of right now, spirv doesn't support library shaders.
        // We will need to compile all shaders at once by constructing them manually.
        if (api == Rhi::GraphicsAPI::Vulkan &&
            desc.IsLibraryShader())
        {
            m_CompiledBlob = std::move(m_SourceCodeBlob);
            return;
        }

        Compile(assetStorage);
        Validate(api, desc);
    }

    ShaderCompilerLibrary ShaderCompilerLibrary::SpirvCompileLinkWorkaround(
        Device&                         rhiDevice,
        std::span<const ShaderBytecode> shaderCodes,
        const ShaderCompileDesc&        desc,
        Asset::Storage*                 assetStorage)
    {
        std::stringstream SourceCode;
        for (const auto& ShaderCode : shaderCodes)
        {
            SourceCode << StringView(std::bit_cast<const char*>(ShaderCode.GetBytecode()), ShaderCode.GetSize());
            SourceCode << "\n";
        }
        return ShaderCompilerLibrary(rhiDevice, SourceCode.str(), desc, assetStorage);
    }

    ShaderBytecode ShaderCompilerLibrary::GetBytecode() const
    {
        if (!m_CompiledBlob)
        {
            return {};
        }

        std::byte* compiledShaderCode = static_cast<std::byte*>(m_CompiledBlob->GetBufferPointer());

        size_t codeSize   = m_CompiledBlob->GetBufferSize();
        auto   outCodePtr = std::make_unique<std::byte[]>(codeSize);

        std::copy(compiledShaderCode, compiledShaderCode + codeSize, outCodePtr.get());
        return ShaderBytecode(outCodePtr.release(), codeSize, m_ShaderStage);
    }

    //

    void ShaderCompilerLibrary::LoadDxc()
    {
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcValidator, IID_PPV_ARGS(&m_Validator)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_Utils)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_Compiler)));
        ShaderUtil::ThrowShaderException(m_Utils->CreateDefaultIncludeHandler(&m_DefaultIncludeHandler));
    }

    void ShaderCompilerLibrary::LoadSourceCode(
        StringView shaderSource)
    {
        ShaderUtil::ThrowShaderException(m_Utils->CreateBlobFromPinned(
            shaderSource.data(),
            static_cast<uint32_t>(shaderSource.size()),
            DXC_CP_UTF8,
            &m_SourceCodeBlob));
    }

    void ShaderCompilerLibrary::Compile(
        Asset::Storage* assetStorage)
    {
        DxcBuffer dxcBuffer{
            .Ptr      = m_SourceCodeBlob->GetBufferPointer(),
            .Size     = m_SourceCodeBlob->GetBufferSize(),
            .Encoding = DXC_CP_ACP
        };

        CComPtr<IDxcResult> result;
        {
            HRESULT hr = S_OK;
            /*if (assetStorage) [[likely]]
            {
                ShaderIncludeHandler includeHandler(
                    m_Utils.Get(),
                    ShaderUtil::GetComPtr(m_DefaultIncludeHandler),
                    assetStorage);

                hr = m_Compiler->Compile(
                    &dxcBuffer,
                    m_CompileOptions.FinalOptions.data(),
                    static_cast<uint32_t>(m_CompileOptions.FinalOptions.size()),
                    &includeHandler,
                    IID_PPV_ARGS(&result));
            }
            else*/
            {
                hr = m_Compiler->Compile(
                    &dxcBuffer,
                    m_CompileOptions.FinalOptions.data(),
                    static_cast<uint32_t>(m_CompileOptions.FinalOptions.size()),
                    nullptr,
                    IID_PPV_ARGS(&result));
            }
            ShaderUtil::ThrowShaderException(hr);
        }
        m_SourceCodeBlob = nullptr;

        HRESULT status;
        if (FAILED(result->GetStatus(&status)))
        {
            Log::Rhi().Error("Failed to get the status of the compilation.");
            return;
        }

        if (FAILED(status))
        {
            CComPtr<IDxcBlobUtf8> error;
            if (SUCCEEDED(result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error), nullptr)) &&
                (error &&
                 error->GetStringLength() > 0))
            {
                Log::Rhi().Error(StringView(error->GetStringPointer(), error->GetStringLength()));
            }
            else
            {
                Log::Rhi().Error("Failed to get the error buffer.");
            }
            return;
        }

        ShaderUtil::ThrowShaderException(
            result->GetOutput(
                DXC_OUT_OBJECT,
                IID_PPV_ARGS(&m_CompiledBlob),
                nullptr));
    }

    //

    void ShaderCompilerLibrary::Validate(
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

    Co::result<ShaderBytecode> ShaderCompiler::CompileAsync(
        Co::executor_tag,
        Co::executor&            executor,
        Device&                  rhiDevice,
        StringView               shaderSource,
        const ShaderCompileDesc& desc,
        Asset::Storage*          assetStorage)
    {
        co_return Compile(rhiDevice, shaderSource, desc, assetStorage);
    }

    ShaderBytecode ShaderCompiler::Compile(
        Device&                  rhiDevice,
        StringView               shaderSource,
        const ShaderCompileDesc& desc,
        Asset::Storage*          assetStorage)
    {
        ShaderCompilerLibrary Library(
            rhiDevice,
            shaderSource,
            desc,
            assetStorage);
        return Library.GetBytecode();
    }
} // namespace Ame::Rhi