#include <map>

#include <Rhi/Device/Device.hpp>

#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Shader/Shader.Handler.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    template<typename Ty>
    using CComPtr = ShaderUtil::CComPtr<Ty>;

    //

    /// <summary>
    /// Precompile the shader.
    /// </summary>
    [[nodiscard]] static CComPtr<IDxcBlob> CompileShader(
        IDxcCompiler3*            Compiler,
        IDxcUtils*                Utils,
        std::span<const wchar_t*> Options,
        IDxcBlobEncoding*         ShaderCodeBlob,
        Asset::Storage*           AssetStorage)
    {
        CComPtr<IDxcIncludeHandler> DefaultIncludeHandler;
        ShaderUtil::ThrowShaderException(Utils->CreateDefaultIncludeHandler(&DefaultIncludeHandler));

        DxcBuffer Buffer{
            .Ptr      = ShaderCodeBlob->GetBufferPointer(),
            .Size     = ShaderCodeBlob->GetBufferSize(),
            .Encoding = DXC_CP_ACP
        };

        CComPtr<IDxcResult> Result;
        {
            HRESULT Hr = S_OK;
            if (AssetStorage)
            {
                ShaderIncludeHandler IncludeHandler(
                    Utils,
                    ShaderUtil::GetComPtr(DefaultIncludeHandler),
                    AssetStorage);
                Hr = Compiler->Compile(
                    &Buffer,
                    Options.data(),
                    uint32_t(Options.size()),
                    &IncludeHandler,
                    IID_PPV_ARGS(&Result));
            }
            else
            {
                Hr = Compiler->Compile(
                    &Buffer,
                    Options.data(),
                    uint32_t(Options.size()),
                    nullptr,
                    IID_PPV_ARGS(&Result));
            }

            ShaderUtil::ThrowShaderException(Hr);

            CComPtr<IDxcBlobUtf8> Error;
            if (SUCCEEDED(Result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&Error), nullptr)) &&
                (Error &&
                 Error->GetStringLength() > 0))
            {
                size_t StrLen = Error->GetStringLength();
                Log::Rhi().Error(StringView(Error->GetStringPointer(), StrLen));
                return {};
            }
        }

        CComPtr<IDxcBlob> Data;
        ShaderUtil::ThrowShaderException(
            Result->GetOutput(
                DXC_OUT_OBJECT,
                IID_PPV_ARGS(&Data),
                nullptr));

        return Data;
    }

    /// <summary>
    /// Validate the shader. (D3D12 only), returns true always for other graphics api
    /// </summary>
    [[nodiscard]] static bool ValidateShader(
        GraphicsAPI        Api,
        IDxcValidator*     Validator,
        IDxcUtils*         Utils,
        CComPtr<IDxcBlob>& Data)
    {
        if (Api == GraphicsAPI::DirectX12)
        {
            CComPtr<IDxcOperationResult> OperationResult;
            ShaderUtil::ThrowShaderException(Validator->Validate(
                ShaderUtil::GetComPtr(Data),
                DxcValidatorFlags_InPlaceEdit,
                &OperationResult));

            HRESULT Status;
            ShaderUtil::ThrowShaderException(OperationResult->GetStatus(&Status));

            if (FAILED(Status))
            {
                CComPtr<IDxcBlobEncoding> Error;
                CComPtr<IDxcBlobUtf8>     ErrorUtf8;

                OperationResult->GetErrorBuffer(&Error);
                Utils->GetBlobAsUtf8(ShaderUtil::GetComPtr(Error), &ErrorUtf8);

                Log::Rhi().Error(StringView(ErrorUtf8->GetStringPointer(), ErrorUtf8->GetBufferSize()));
                return false;
            }
            else
            {
                Data = nullptr;
                ShaderUtil::ThrowShaderException(OperationResult->GetResult(&Data));
            }
        }
        return true;
    }

    //

    /// <summary>
    /// Load shader blob from string.
    /// </summary>
    [[nodiscard]] static CComPtr<IDxcBlobEncoding> LoadShaderFromString(
        IDxcUtils* Utils,
        StringView ShaderSource)
    {
        CComPtr<IDxcBlobEncoding> ShaderCodeBlob;
        ShaderUtil::ThrowShaderException(Utils->CreateBlobFromPinned(
            ShaderSource.data(),
            uint32_t(ShaderSource.size()),
            DXC_CP_UTF8,
            &ShaderCodeBlob));
        return ShaderCodeBlob;
    }

    //

    Co::result<ShaderBytecode> ShaderCompiler::CompileAsync(
        Co::executor_tag,
        Co::executor&            Executor,
        Device&                  RhiDevice,
        StringView               ShaderSource,
        const ShaderCompileDesc& CompileDesc,
        Asset::Storage*          AssetStorage)
    {
        co_return Compile(RhiDevice, ShaderSource, CompileDesc, AssetStorage);
    }

    ShaderBytecode ShaderCompiler::Compile(
        Device&                  RhiDevice,
        StringView               ShaderSource,
        const ShaderCompileDesc& Desc,
        Asset::Storage*          AssetStorage)
    {
        CComPtr<IDxcValidator> Validator;
        CComPtr<IDxcUtils>     Utils;
        CComPtr<IDxcCompiler3> Compiler;

        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcValidator, IID_PPV_ARGS(&Validator)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils)));
        ShaderUtil::ThrowShaderException(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)));

        //

        auto ShaderCodeBlob = LoadShaderFromString(ShaderUtil::GetComPtr(Utils), ShaderSource);
        auto Options        = CompileShaderOption(RhiDevice, Desc);
        auto Data           = CompileShader(
            ShaderUtil::GetComPtr(Compiler),
            ShaderUtil::GetComPtr(Utils),
            Options.FinalOptions,
            ShaderUtil::GetComPtr(ShaderCodeBlob),
            AssetStorage);

        ShaderBytecode Shader;
        if (Data && ValidateShader(Options.Api, ShaderUtil::GetComPtr(Validator), ShaderUtil::GetComPtr(Utils), Data))
        {
            uint8_t* CompiledShaderCode = static_cast<uint8_t*>(Data->GetBufferPointer());

            size_t CodeSize = Data->GetBufferSize();
            auto   CodePtr  = std::make_unique<uint8_t[]>(CodeSize);

            std::copy(CompiledShaderCode, CompiledShaderCode + CodeSize, CodePtr.get());
            Shader = ShaderBytecode(CodePtr.release(), CodeSize, Desc.Stage);
        }

        return Shader;
    }
} // namespace Ame::Rhi