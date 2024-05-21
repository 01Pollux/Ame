#include <Rhi/Shader/Shader.Compiler.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    ShaderCompilerLibrary::ShaderCompilerLibrary(
        Device&                  RhiDevice,
        StringView               ShaderSource,
        const ShaderCompileDesc& Desc,
        Asset::Storage*          AssetStorage) :
        m_CompileOptions(RhiDevice, Desc),
        m_ShaderStage(Desc.Stage)
    {
        LoadDxc();
        LoadSourceCode(ShaderSource);
        Compile(AssetStorage);
        Validate(RhiDevice.GetGraphicsAPI(), Desc);
    }

    ShaderBytecode ShaderCompilerLibrary::GetBytecode() const
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

    void ShaderCompilerLibrary::LoadDxc()
    {
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcValidator, IID_PPV_ARGS(&m_Validator)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_Utils)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_Compiler)));
        ShaderUtil::ThrowShaderException(m_Utils->CreateDefaultIncludeHandler(&m_DefaultIncludeHandler));
    }

    void ShaderCompilerLibrary::LoadSourceCode(
        StringView ShaderSource)
    {
        ShaderUtil::ThrowShaderException(m_Utils->CreateBlobFromPinned(
            ShaderSource.data(),
            static_cast<uint32_t>(ShaderSource.size()),
            DXC_CP_UTF8,
            &m_SourceCodeBlob));
    }

    void ShaderCompilerLibrary::Compile(
        Asset::Storage* AssetStorage)
    {
        DxcBuffer Buffer{
            .Ptr      = m_SourceCodeBlob->GetBufferPointer(),
            .Size     = m_SourceCodeBlob->GetBufferSize(),
            .Encoding = DXC_CP_ACP
        };

        CComPtr<IDxcResult> Result;
        {
            HRESULT Hr = S_OK;
            if (AssetStorage) [[likely]]
            {
                ShaderIncludeHandler IncludeHandler(
                    m_Utils.Get(),
                    ShaderUtil::GetComPtr(m_DefaultIncludeHandler),
                    AssetStorage);

                Hr = m_Compiler->Compile(
                    &Buffer,
                    m_CompileOptions.FinalOptions.data(),
                    static_cast<uint32_t>(m_CompileOptions.FinalOptions.size()),
                    &IncludeHandler,
                    IID_PPV_ARGS(&Result));
            }
            else
            {
                Hr = m_Compiler->Compile(
                    &Buffer,
                    m_CompileOptions.FinalOptions.data(),
                    static_cast<uint32_t>(m_CompileOptions.FinalOptions.size()),
                    nullptr,
                    IID_PPV_ARGS(&Result));
            }
            ShaderUtil::ThrowShaderException(Hr);
        }

        HRESULT Status;
        if (FAILED(Result->GetStatus(&Status)))
        {
            Log::Rhi().Error("Failed to get the status of the compilation.");
            return;
        }

        if (FAILED(Status))
        {
            CComPtr<IDxcBlobUtf8> Error;
            if (SUCCEEDED(Result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&Error), nullptr)) &&
                (Error &&
                 Error->GetStringLength() > 0))
            {
                size_t StrLen = Error->GetStringLength();
                Log::Rhi().Error(StringView(Error->GetStringPointer(), StrLen));
                return;
            }
        }

        ShaderUtil::ThrowShaderException(
            Result->GetOutput(
                DXC_OUT_OBJECT,
                IID_PPV_ARGS(&m_CompiledBlob),
                nullptr));
    }

    void ShaderCompilerLibrary::Validate(
        GraphicsAPI              Api,
        const ShaderCompileDesc& Desc)
    {
        using namespace EnumBitOperators;

        if (Api != GraphicsAPI::DirectX12)
        {
            return;
        }

        if (!m_CompiledBlob)
        {
            return;
        }

        if (!Desc.ShouldValidate())
        {
            return;
        }

        CComPtr<IDxcOperationResult> OperationResult;
        ShaderUtil::ThrowShaderException(m_Validator->Validate(
            ShaderUtil::GetComPtr(m_CompiledBlob),
            DxcValidatorFlags_InPlaceEdit,
            &OperationResult));

        HRESULT Status;
        ShaderUtil::ThrowShaderException(OperationResult->GetStatus(&Status));

        if (FAILED(Status))
        {
            CComPtr<IDxcBlobEncoding> Error;
            CComPtr<IDxcBlobUtf8>     ErrorUtf8;

            OperationResult->GetErrorBuffer(&Error);
            if (Error)
            {
                m_Utils->GetBlobAsUtf8(ShaderUtil::GetComPtr(Error), &ErrorUtf8);
            }
            else
            {
                Log::Rhi().Error("Failed to get the error buffer.");
            }

            if (ErrorUtf8)
            {
                Log::Rhi().Error(StringView(ErrorUtf8->GetStringPointer(), ErrorUtf8->GetBufferSize()));
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
            ShaderUtil::ThrowShaderException(OperationResult->GetResult(&m_CompiledBlob));
        }
    }

    //

    Co::result<ShaderBytecode> ShaderCompiler::CompileAsync(
        Co::executor_tag,
        Co::executor&            Executor,
        Device&                  RhiDevice,
        StringView               ShaderSource,
        const ShaderCompileDesc& Desc,
        Asset::Storage*          AssetStorage)
    {
        co_return Compile(RhiDevice, ShaderSource, Desc, AssetStorage);
    }

    ShaderBytecode ShaderCompiler::Compile(
        Device&                  RhiDevice,
        StringView               ShaderSource,
        const ShaderCompileDesc& Desc,
        Asset::Storage*          AssetStorage)
    {
        ShaderCompilerLibrary Library(
            RhiDevice,
            ShaderSource,
            Desc,
            AssetStorage);
        return Library.GetBytecode();
    }
} // namespace Ame::Rhi