#include <Rhi/Shader.hpp>
#include <FileSystem/Path.hpp>

#include <Rhi/Device.hpp>
#include <Rhi/Device/SprivBinding.hpp>

#ifdef AME_PLATFORM_WINDOWS
#include <d3d12shader.h>
#include <dxcapi.h>
#include <wrl.h>

template<typename Ty>
using CComPtr = Microsoft::WRL::ComPtr<Ty>;

template<typename Ty>
[[nodiscard]] static inline Ty* GetComPtr(
    CComPtr<Ty>& Object)
{
    return Object.Get();
}

#else
#include <dxc/dxcapi.h>

template<typename Ty>
[[nodiscard]] static inline Ty* GetComPtr(
    CComPtr<Ty>& Object)
{
    return Object;
}

#endif

#include <map>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    using namespace Enum::bitwise_operators;

    static void ThrowShaderException(
        HRESULT Result)
    {
        if (FAILED(Result))
        {
            throw std::runtime_error(std::format("Failed to compile shader: {:X}", Result));
        }
    }

    //

    /// <summary>
    /// Get the shader entry point.
    /// </summary>
    [[nodiscard]] const char* GetShaderEntryPoint(
        nri::StageBits Stage)
    {
        switch (Stage)
        {
        case nri::StageBits::COMPUTE_SHADER:
            return "CS_Main";
        case nri::StageBits::VERTEX_SHADER:
            return "VS_Main";
        case nri::StageBits::TESS_CONTROL_SHADER:
            return "HS_Main";
        case nri::StageBits::TESS_EVALUATION_SHADER:
            return "DS_Main";
        case nri::StageBits::GEOMETRY_SHADER:
            return "GS_Main";
        case nri::StageBits::FRAGMENT_SHADER:
            return "PS_Main";
        case nri::StageBits::MESH_CONTROL_SHADER:
            return "AS_Main";
        case nri::StageBits::MESH_EVALUATION_SHADER:
            return "MS_Main";
        default:
            std::unreachable();
        }
    }

    /// <summary>
    /// Get the shader entry point.
    /// </summary>
    [[nodiscard]] const wchar_t* GetShaderEntryPointWide(
        nri::StageBits Stage)
    {
        switch (Stage)
        {
        case nri::StageBits::COMPUTE_SHADER:
            return L"CS_Main";
        case nri::StageBits::VERTEX_SHADER:
            return L"VS_Main";
        case nri::StageBits::TESS_CONTROL_SHADER:
            return L"HS_Main";
        case nri::StageBits::TESS_EVALUATION_SHADER:
            return L"DS_Main";
        case nri::StageBits::GEOMETRY_SHADER:
            return L"GS_Main";
        case nri::StageBits::FRAGMENT_SHADER:
            return L"PS_Main";
        case nri::StageBits::MESH_CONTROL_SHADER:
            return L"AS_Main";
        case nri::StageBits::MESH_EVALUATION_SHADER:
            return L"MS_Main";
        default:
            std::unreachable();
        }
    }

    /// <summary>
    /// Get the shader define macro.
    /// </summary>
    [[nodiscard]] static const wchar_t* GetShaderMacro(
        nri::StageBits Stage)
    {
        switch (Stage)
        {
        case nri::StageBits::COMPUTE_SHADER:
            return L"COMPUTE_SHADER=1";
        case nri::StageBits::VERTEX_SHADER:
            return L"VERTEX_SHADER=1";
        case nri::StageBits::TESS_CONTROL_SHADER:
            return L"HULL_SHADER=1";
        case nri::StageBits::TESS_EVALUATION_SHADER:
            return L"DOMAIN_SHADER=1";
        case nri::StageBits::GEOMETRY_SHADER:
            return L"GEOMETRY_SHADER=1";
        case nri::StageBits::FRAGMENT_SHADER:
            return L"PIXEL_SHADER=1";
        case nri::StageBits::MESH_CONTROL_SHADER:
            return L"AMPLIFICATION_SHADER=1";
        case nri::StageBits::MESH_EVALUATION_SHADER:
            return L"MESHSHADER_Main";
        default:
            std::unreachable();
        }
    }

    /// <summary>
    /// Get the shader entry model.
    /// </summary>
    [[nodiscard]] static String GetShaderModel(
        nri::StageBits Stage,
        ShaderProfile  Profile)
    {
        String Model;

        switch (Stage)
        {
        case nri::StageBits::COMPUTE_SHADER:
            Model = L"cs";
            break;
        case nri::StageBits::VERTEX_SHADER:
            Model = L"vs";
            break;
        case nri::StageBits::GEOMETRY_SHADER:
            Model = L"gs";
            break;
        case nri::StageBits::TESS_CONTROL_SHADER:
            Model = L"hs";
            break;
        case nri::StageBits::TESS_EVALUATION_SHADER:
            Model = L"ds";
            break;
        case nri::StageBits::FRAGMENT_SHADER:
            Model = L"ps";
            break;
        case nri::StageBits::MESH_CONTROL_SHADER:
            Model = L"as";
            break;
        case nri::StageBits::MESH_EVALUATION_SHADER:
            Model = L"ms";
            break;
        default:
            std::unreachable();
        }

        switch (Profile)
        {
        case ShaderProfile::_6_0:
            Model += L"_6_0";
            break;
        case ShaderProfile::_6_1:
            Model += L"_6_1";
            break;
        case ShaderProfile::_6_2:
            Model += L"_6_2";
            break;
        case ShaderProfile::_6_3:
            Model += L"_6_3";
            break;
        case ShaderProfile::_6_4:
            Model += L"_6_4";
            break;
        case ShaderProfile::_6_5:
            Model += L"_6_5";
            break;
        case ShaderProfile::_6_6:
            Model += L"_6_6";
            break;
        default:
            std::unreachable();
        }

        return Model;
    }

    //

    /// <summary>
    /// Add SPIRV extensions for the shader compiler.
    /// </summary>
    static void AddSpirvExtensions(
        std::span<const ShaderVulkanExtension> Extensions,
        std::vector<const wchar_t*>&           Options)
    {
        for (auto Extension : Extensions)
        {
            switch (Extension)
            {
            case ShaderVulkanExtension::KHR:
                Options.emplace_back(L"-fspv-extension=KHR");
                break;
            case ShaderVulkanExtension::SPV_KHR_16bit_storage:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_16bit_storage");
                break;
            case ShaderVulkanExtension::SPV_KHR_device_group:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_device_group");
                break;
            case ShaderVulkanExtension::SPV_KHR_fragment_shading_rate:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_fragment_shading_rate");
                break;
            case ShaderVulkanExtension::SPV_KHR_non_semantic_info:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_non_semantic_info");
                break;
            case ShaderVulkanExtension::SPV_KHR_multiview:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_multiview");
                break;
            case ShaderVulkanExtension::SPV_KHR_shader_draw_parameters:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_shader_draw_parameters");
                break;
            case ShaderVulkanExtension::SPV_KHR_post_depth_coverage:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_post_depth_coverage");
                break;
            case ShaderVulkanExtension::SPV_KHR_ray_tracing:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_ray_tracing");
                break;
            case ShaderVulkanExtension::SPV_KHR_shader_clock:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_shader_clock");
                break;
            case ShaderVulkanExtension::SPV_EXT_demote_to_helper_invocation:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_demote_to_helper_invocation");
                break;
            case ShaderVulkanExtension::SPV_EXT_descriptor_indexing:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_descriptor_indexing");
                break;
            case ShaderVulkanExtension::SPV_EXT_fragment_fully_covered:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_fragment_fully_covered");
                break;
            case ShaderVulkanExtension::SPV_EXT_fragment_invocation_density:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_fragment_invocation_density");
                break;
            case ShaderVulkanExtension::SPV_EXT_mesh_shader:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_mesh_shader");
                break;
            case ShaderVulkanExtension::SPV_EXT_shader_stencil_export:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_shader_stencil_export");
                break;
            case ShaderVulkanExtension::SPV_EXT_shader_viewport_index_layer:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_shader_viewport_index_layer");
                break;
            case ShaderVulkanExtension::SPV_AMD_gpu_shader_half_float:
                Options.emplace_back(L"-fspv-extension=SPV_AMD_gpu_shader_half_float");
                break;
            case ShaderVulkanExtension::SPV_AMD_shader_early_and_late_fragment_tests:
                Options.emplace_back(L"-fspv-extension=SPV_AMD_shader_early_and_late_fragment_tests");
                break;
            case ShaderVulkanExtension::SPV_AMD_shader_explicit_vertex_parameter:
                Options.emplace_back(L"-fspv-extension=SPV_AMD_shader_explicit_vertex_parameter");
                break;
            case ShaderVulkanExtension::SPV_GOOGLE_hlsl_functionality1:
                Options.emplace_back(L"-fspv-extension=SPV_GOOGLE_hlsl_functionality1");
                break;
            case ShaderVulkanExtension::SPV_GOOGLE_user_type:
                Options.emplace_back(L"-fspv-extension=SPV_GOOGLE_user_type");
                break;
            case ShaderVulkanExtension::SPV_NV_ray_tracing:
                Options.emplace_back(L"-fspv-extension=SPV_NV_ray_tracing");
                break;
            case ShaderVulkanExtension::SPV_NV_mesh_shader:
                Options.emplace_back(L"-fspv-extension=SPV_NV_mesh_shader");
                break;
            case ShaderVulkanExtension::SPV_KHR_ray_query:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_ray_query");
                break;
            case ShaderVulkanExtension::SPV_EXT_shader_image_int64:
                Options.emplace_back(L"-fspv-extension=SPV_EXT_shader_image_int64");
                break;
            case ShaderVulkanExtension::SPV_KHR_physical_storage_buffer:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_physical_storage_buffer");
                break;
            case ShaderVulkanExtension::SPV_KHR_vulkan_memory_model:
                Options.emplace_back(L"-fspv-extension=SPV_KHR_vulkan_memory_model");
                break;
            }
        }
    }

    /// <summary>
    /// Add SPIRV layout for the shader compiler.
    /// </summary>
    static void AddSpirvLayout(
        const ShaderCompileDesc&     Desc,
        std::vector<String>&         RegisterShift,
        std::vector<const wchar_t*>& Options)
    {
        switch (Desc.VulkanMemoryLayout)
        {
        case ShaderVulkanMemLayout::Dx:
            Options.emplace_back(L"-fvk-use-dx-layout");
            break;
        case ShaderVulkanMemLayout::Gl:
            Options.emplace_back(L"-fvk-use-gl-layout");
            break;
        case ShaderVulkanMemLayout::Scalar:
            Options.emplace_back(L"-fvk-use-scalar-layout");
            break;
        }

        constexpr auto p = std::is_same_v<String::std_string_view_type, std::wstring_view>;


        RegisterShift.emplace_back(String::formatted(L"{}", DefaultSpirvBindingOffset.textureOffset));
        RegisterShift.emplace_back(String::formatted(L"{}", DefaultSpirvBindingOffset.storageTextureAndBufferOffset));
        RegisterShift.emplace_back(String::formatted(L"{}", DefaultSpirvBindingOffset.samplerOffset));
        RegisterShift.emplace_back(String::formatted(L"{}", DefaultSpirvBindingOffset.constantBufferOffset));

        const wchar_t* TShift = RegisterShift[0].c_str();
        const wchar_t* UShift = RegisterShift[1].c_str();
        const wchar_t* SShift = RegisterShift[2].c_str();
        const wchar_t* BShift = RegisterShift[3].c_str();

        constexpr const wchar_t* Spaces[]{
            L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7",
            L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15"
        };

        for (uint32_t i = 0; i < 16; i++)
        {
            auto Space = Spaces[i];

            Options.emplace_back(L"-fvk-t-shift");
            Options.emplace_back(TShift);
            Options.emplace_back(Space);

            Options.emplace_back(L"-fvk-u-shift");
            Options.emplace_back(UShift);
            Options.emplace_back(Space);

            Options.emplace_back(L"-fvk-s-shift");
            Options.emplace_back(SShift);
            Options.emplace_back(Space);

            Options.emplace_back(L"-fvk-b-shift");
            Options.emplace_back(BShift);
            Options.emplace_back(Space);
        }
    }

    //

    /// <summary>
    /// Load shader blob from string.
    /// </summary>
    [[nodiscard]] static CComPtr<IDxcBlobEncoding> LoadShaderFromString(
        IDxcUtils*   Utils,
        StringU8View ShaderSource)
    {
        CComPtr<IDxcBlobEncoding> ShaderCodeBlob;
        ThrowShaderException(Utils->CreateBlobFromPinned(
            ShaderSource.data(),
            uint32_t(ShaderSource.size()),
            DXC_CP_UTF8,
            &ShaderCodeBlob));
        return ShaderCodeBlob;
    }

    struct CompileShaderOption
    {
        std::vector<const wchar_t*> FinalOptions;

        CompileShaderOption(
            GraphicsAPI              Api,
            const ShaderCompileDesc& Desc) :
            Model(GetShaderModel(Desc.Stage, Desc.Profile)),
            DefineMacro(GetShaderMacro(Desc.Stage))
        {
            FinalOptions = {
                DXC_ARG_OPTIMIZATION_LEVEL3,
                DXC_ARG_ALL_RESOURCES_BOUND,
                L"-HV 2021",
                L"-E", GetShaderEntryPointWide(Desc.Stage),
                L"-T", Model.c_str(),
                L"-D", DefineMacro.c_str()
            };

#ifndef AME_DIST
            if ((Desc.Flags & ShaderFlags::Debug) != ShaderFlags::None)
            {
                FinalOptions.emplace_back(DXC_ARG_DEBUG);
                FinalOptions.emplace_back(L"-Qembed_debug");
            }
            else
#endif
            {
                FinalOptions.emplace_back(L"-Qstrip_debug");
            }

            switch (Api)
            {
            case GraphicsAPI::DirectX12:
            {
                FinalOptions.emplace_back(L"-D AME_SHADER_COMPILER_D3D12=1");
                break;
            }
            case GraphicsAPI::Vulkan:
            {
                FinalOptions.emplace_back(L"-D AME_SHADER_COMPILER_SPIRV=1");
                FinalOptions.emplace_back(L"-spirv");

                AddSpirvExtensions(Desc.SpirvExtensions, FinalOptions);
                AddSpirvLayout(Desc, RegisterShift, FinalOptions);

                break;
            }
            }

            MacrosCombined.reserve(Desc.Defines.size());
            FinalOptions.reserve(FinalOptions.size() + Desc.Defines.size());

            for (auto& [Key, Value] : Desc.Defines)
            {
                MacrosCombined.emplace_back(String::formatted(
                    L"{}={}",
                    Key,
                    Value.empty() ? L"1" : Value.view()));
            }

            for (auto& Macro : MacrosCombined)
            {
                FinalOptions.emplace_back(L"-D");
                FinalOptions.emplace_back(Macro.c_str());
            }
        }

    private:
        String              Model;
        String              DefineMacro;
        std::vector<String> RegisterShift;
        std::vector<String> MacrosCombined;
    };

    /// <summary>
    /// Precompile the shader.
    /// </summary>
    [[nodiscard]] static CComPtr<IDxcBlob> PrecompileShader(
        IDxcCompiler3*            Compiler,
        IDxcUtils*                Utils,
        std::span<const wchar_t*> Options,
        IDxcBlobEncoding*         ShaderCodeBlob)
    {
        CComPtr<IDxcIncludeHandler> DefaultIncludeHandler;
        ThrowShaderException(Utils->CreateDefaultIncludeHandler(&DefaultIncludeHandler));

        DxcBuffer Buffer{
            .Ptr      = ShaderCodeBlob->GetBufferPointer(),
            .Size     = ShaderCodeBlob->GetBufferSize(),
            .Encoding = DXC_CP_ACP
        };

        CComPtr<IDxcResult> Result;
        {
            ThrowShaderException(Compiler->Compile(
                &Buffer,
                Options.data(),
                uint32_t(Options.size()),
                nullptr,
                IID_PPV_ARGS(&Result)));

            CComPtr<IDxcBlobUtf8> Error;
            if (SUCCEEDED(Result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&Error), nullptr)) &&
                (Error &&
                 Error->GetStringLength() > 0))
            {
                size_t StrLen = Error->GetStringLength();
                Log::Rhi().Error(StringU8View(Error->GetStringPointer(), StrLen));
                return {};
            }
        }

        CComPtr<IDxcBlob> Data;
        ThrowShaderException(Result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&Data), nullptr));

        return Data;
    }

    /// <summary>
    /// Validate the shader. (D3D12 only), returns true always for other graphics api
    /// </summary>
    [[nodiscard]] static bool ValidateShader(
        GraphicsAPI        GraphicsApi,
        IDxcValidator*     Validator,
        IDxcUtils*         Utils,
        CComPtr<IDxcBlob>& Data)
    {
        if (GraphicsApi == GraphicsAPI::DirectX12)
        {
            CComPtr<IDxcOperationResult> OperationResult;
            ThrowShaderException(Validator->Validate(
                GetComPtr(Data),
                DxcValidatorFlags_InPlaceEdit,
                &OperationResult));

            HRESULT Status;
            ThrowShaderException(OperationResult->GetStatus(&Status));

            if (FAILED(Status))
            {
                CComPtr<IDxcBlobEncoding> Error;
                CComPtr<IDxcBlobUtf8>     ErrorUtf8;

                OperationResult->GetErrorBuffer(&Error);
                Utils->GetBlobAsUtf8(GetComPtr(Error), &ErrorUtf8);

                Log::Rhi().Error(StringU8View(ErrorUtf8->GetStringPointer(), ErrorUtf8->GetBufferSize()));
                return false;
            }
            else
            {
                Data = nullptr;
                ThrowShaderException(OperationResult->GetResult(&Data));
            }
        }
        return true;
    }

    //

    ShaderBytecode ShaderBytecode::Compile(
        GraphicsAPI              Api,
        StringU8View             ShaderSource,
        const ShaderCompileDesc& Desc)
    {
        CComPtr<IDxcValidator> Validator;
        CComPtr<IDxcUtils>     Utils;
        CComPtr<IDxcCompiler3> Compiler;

        ThrowShaderException(DxcCreateInstance(CLSID_DxcValidator, IID_PPV_ARGS(&Validator)));
        ThrowShaderException(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils)));
        ThrowShaderException(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler)));

        //

        auto ShaderCodeBlob = LoadShaderFromString(GetComPtr(Utils), ShaderSource);
        auto Options        = CompileShaderOption(Api, Desc);
        auto Data           = PrecompileShader(GetComPtr(Compiler), GetComPtr(Utils), Options.FinalOptions, GetComPtr(ShaderCodeBlob));

        ShaderBytecode Shader;
        if (Data && ValidateShader(Api, GetComPtr(Validator), GetComPtr(Utils), Data))
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