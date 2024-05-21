#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Device/SprivBinding.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Get the shader entry model.
    /// </summary>
    [[nodiscard]] static WideString GetShaderTargetProfile(
        bool          IsLibrary,
        ShaderType    Stage,
        ShaderProfile Profile)
    {
        WideString Model = IsLibrary ? L"lib" : L"";

        if (!IsLibrary)
        {
            switch (Stage)
            {
            case ShaderType::COMPUTE_SHADER:
                Model = L"cs";
                break;
            case ShaderType::VERTEX_SHADER:
                Model = L"vs";
                break;
            case ShaderType::GEOMETRY_SHADER:
                Model = L"gs";
                break;
            case ShaderType::TESS_CONTROL_SHADER:
                Model = L"hs";
                break;
            case ShaderType::TESS_EVALUATION_SHADER:
                Model = L"ds";
                break;
            case ShaderType::FRAGMENT_SHADER:
                Model = L"ps";
                break;
            case ShaderType::MESH_CONTROL_SHADER:
                Model = L"as";
                break;
            case ShaderType::MESH_EVALUATION_SHADER:
                Model = L"ms";
                break;
            default:
                std::unreachable();
            }
        }

        switch (Profile)
        {
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

    WideString CompileShaderOption::GetTargetProfile(
        ShaderType    Stage,
        ShaderProfile Profile)
    {
        return GetShaderTargetProfile(false, Stage, Profile);
    }

    WideString CompileShaderOption::GetTargetProfile(
        ShaderCompileFlags Flags,
        ShaderType         Stage,
        ShaderProfile      Profile)
    {
        using namespace EnumBitOperators;

        //

        bool IsLibrary = (Flags & ShaderCompileFlags::LibraryShader) == ShaderCompileFlags::LibraryShader;
        return GetShaderTargetProfile(IsLibrary, Stage, Profile);
    }

    //

    /// <summary>
    /// Get the shader define macro.
    /// </summary>
    [[nodiscard]] static const wchar_t* GetShaderMacro(
        ShaderType Stage)
    {
        switch (Stage)
        {
        case ShaderType::COMPUTE_SHADER:
            return L"-DCOMPUTE_SHADER=1";
        case ShaderType::VERTEX_SHADER:
            return L"-DVERTEX_SHADER=1";
        case ShaderType::TESS_CONTROL_SHADER:
            return L"-DHULL_SHADER=1";
        case ShaderType::TESS_EVALUATION_SHADER:
            return L"-DDOMAIN_SHADER=1";
        case ShaderType::GEOMETRY_SHADER:
            return L"-DGEOMETRY_SHADER=1";
        case ShaderType::FRAGMENT_SHADER:
            return L"-DPIXEL_SHADER=1";
        case ShaderType::MESH_CONTROL_SHADER:
            return L"-DAMPLIFICATION_SHADER=1";
        case ShaderType::MESH_EVALUATION_SHADER:
            return L"-DMESHSHADER_Main";
        default:
            std::unreachable();
        }
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
        std::vector<WideString>&     RegisterShift,
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

        RegisterShift.emplace_back(std::format(L"{}", DefaultSpirvBindingOffset.textureOffset));
        RegisterShift.emplace_back(std::format(L"{}", DefaultSpirvBindingOffset.storageTextureAndBufferOffset));
        RegisterShift.emplace_back(std::format(L"{}", DefaultSpirvBindingOffset.samplerOffset));
        RegisterShift.emplace_back(std::format(L"{}", DefaultSpirvBindingOffset.constantBufferOffset));

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

    CompileShaderOption::CompileShaderOption(
        Device&                  RhiDevice,
        const ShaderCompileDesc& Desc) :
        Api(RhiDevice.GetGraphicsAPI()),
        TargetProfile(GetTargetProfile(Desc.Flags, Desc.Stage, Desc.Profile)),
        EntryPoint(GetShaderEntryPointWide(Desc.Stage)),
        DefineMacro(GetShaderMacro(Desc.Stage))
    {
        using namespace EnumBitOperators;

        //

        bool IsLibrary = (Desc.Flags & ShaderCompileFlags::LibraryShader) == ShaderCompileFlags::LibraryShader;

        auto& RhiDesc = RhiDevice.GetDesc();

        FinalOptions = {
            DXC_ARG_OPTIMIZATION_LEVEL3,
            DXC_ARG_ALL_RESOURCES_BOUND,
            L"-HV 2021",
            L"-E", EntryPoint,
            L"-T", TargetProfile.c_str(),
            DefineMacro.c_str()
        };

        if (Desc.ShouldValidate())
        {
            FinalOptions.emplace_back(L"-Vd");
        }

        if (RhiDesc.isDrawParametersEmulationEnabled)
        {
            FinalOptions.emplace_back(L"-DAME_ENABLE_DRAW_PARAMETERS_EMULATION=1");
        }

#ifndef AME_DIST
        using namespace EnumBitOperators;

        if ((Desc.Flags & ShaderCompileFlags::Debug) != ShaderCompileFlags::None)
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
            FinalOptions.emplace_back(L"-DAME_SHADER_COMPILER_D3D12=1");
            break;
        }
        case GraphicsAPI::Vulkan:
        {
            FinalOptions.emplace_back(L"-DAME_SHADER_COMPILER_SPIRV=1");
            FinalOptions.emplace_back(L"-spirv");
            FinalOptions.emplace_back(L"-fvk-use-dx-layout");

            AddSpirvExtensions(Desc.SpirvExtensions, FinalOptions);
            AddSpirvLayout(Desc, RegisterShift, FinalOptions);

            break;
        }
        }

        MacrosCombined.reserve(Desc.Defines.size());
        FinalOptions.reserve(FinalOptions.size() + Desc.Defines.size());

        for (auto& [Key, Value] : Desc.Defines)
        {
            MacrosCombined.emplace_back(std::format(
                L"-D{}={}",
                Key,
                Value.empty() ? L"1" : Value.data()));
        }

        for (auto& Macro : MacrosCombined)
        {
            FinalOptions.emplace_back(Macro.c_str());
        }
    }
} // namespace Ame::Rhi