#include <Rhi/Shader/Shader.Options.hpp>
#include <Rhi/Device/SprivBinding.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Get the shader entry model.
    /// </summary>
    [[nodiscard]] static WideString GetShaderTargetProfile(
        ShaderCompileStage stage,
        ShaderProfile      profile)
    {
        WideString model;

        switch (stage)
        {
        case ShaderCompileStage::Vertex:
            model = L"vs";
            break;
        case ShaderCompileStage::Domain:
            model = L"ds";
            break;
        case ShaderCompileStage::Hull:
            model = L"hs";
            break;
        case ShaderCompileStage::Geometry:
            model = L"gs";
            break;
        case ShaderCompileStage::Pixel:
            model = L"ps";
            break;
        case ShaderCompileStage::Compute:
            model = L"cs";
            break;
        case ShaderCompileStage::Amplification:
            model = L"as";
            break;
        case ShaderCompileStage::Mesh:
            model = L"ms";
            break;
        case ShaderCompileStage::Library:
            model = L"lib";
            break;
        case ShaderCompileStage::Count:
            break;
        default:
            std::unreachable();
        }

        switch (profile)
        {
        case ShaderProfile::_6_1:
            model += L"_6_1";
            break;
        case ShaderProfile::_6_2:
            model += L"_6_2";
            break;
        case ShaderProfile::_6_3:
            model += L"_6_3";
            break;
        case ShaderProfile::_6_4:
            model += L"_6_4";
            break;
        case ShaderProfile::_6_5:
            model += L"_6_5";
            break;
        case ShaderProfile::_6_6:
            model += L"_6_6";
            break;
        default:
            std::unreachable();
        }

        return model;
    }

    //

    /// <summary>
    /// Add SPIRV stage for the shader compiler.
    /// </summary>
    static void AddSpirvExtensions(
        std::span<const ShaderVulkanExtension> Extensions,
        std::vector<const WideChar*>&          options)
    {
        for (auto extension : Extensions)
        {
            switch (extension)
            {
            case ShaderVulkanExtension::KHR:
                options.emplace_back(L"-fspv-extension=KHR");
                break;
            case ShaderVulkanExtension::SPV_KHR_16bit_storage:
                options.emplace_back(L"-fspv-extension=SPV_KHR_16bit_storage");
                break;
            case ShaderVulkanExtension::SPV_KHR_device_group:
                options.emplace_back(L"-fspv-extension=SPV_KHR_device_group");
                break;
            case ShaderVulkanExtension::SPV_KHR_fragment_shading_rate:
                options.emplace_back(L"-fspv-extension=SPV_KHR_fragment_shading_rate");
                break;
            case ShaderVulkanExtension::SPV_KHR_non_semantic_info:
                options.emplace_back(L"-fspv-extension=SPV_KHR_non_semantic_info");
                break;
            case ShaderVulkanExtension::SPV_KHR_multiview:
                options.emplace_back(L"-fspv-extension=SPV_KHR_multiview");
                break;
            case ShaderVulkanExtension::SPV_KHR_shader_draw_parameters:
                options.emplace_back(L"-fspv-extension=SPV_KHR_shader_draw_parameters");
                break;
            case ShaderVulkanExtension::SPV_KHR_post_depth_coverage:
                options.emplace_back(L"-fspv-extension=SPV_KHR_post_depth_coverage");
                break;
            case ShaderVulkanExtension::SPV_KHR_ray_tracing:
                options.emplace_back(L"-fspv-extension=SPV_KHR_ray_tracing");
                break;
            case ShaderVulkanExtension::SPV_KHR_shader_clock:
                options.emplace_back(L"-fspv-extension=SPV_KHR_shader_clock");
                break;
            case ShaderVulkanExtension::SPV_EXT_demote_to_helper_invocation:
                options.emplace_back(L"-fspv-extension=SPV_EXT_demote_to_helper_invocation");
                break;
            case ShaderVulkanExtension::SPV_EXT_descriptor_indexing:
                options.emplace_back(L"-fspv-extension=SPV_EXT_descriptor_indexing");
                break;
            case ShaderVulkanExtension::SPV_EXT_fragment_fully_covered:
                options.emplace_back(L"-fspv-extension=SPV_EXT_fragment_fully_covered");
                break;
            case ShaderVulkanExtension::SPV_EXT_fragment_invocation_density:
                options.emplace_back(L"-fspv-extension=SPV_EXT_fragment_invocation_density");
                break;
            case ShaderVulkanExtension::SPV_EXT_mesh_shader:
                options.emplace_back(L"-fspv-extension=SPV_EXT_mesh_shader");
                break;
            case ShaderVulkanExtension::SPV_EXT_shader_stencil_export:
                options.emplace_back(L"-fspv-extension=SPV_EXT_shader_stencil_export");
                break;
            case ShaderVulkanExtension::SPV_EXT_shader_viewport_index_layer:
                options.emplace_back(L"-fspv-extension=SPV_EXT_shader_viewport_index_layer");
                break;
            case ShaderVulkanExtension::SPV_AMD_gpu_shader_half_float:
                options.emplace_back(L"-fspv-extension=SPV_AMD_gpu_shader_half_float");
                break;
            case ShaderVulkanExtension::SPV_AMD_shader_early_and_late_fragment_tests:
                options.emplace_back(L"-fspv-extension=SPV_AMD_shader_early_and_late_fragment_tests");
                break;
            case ShaderVulkanExtension::SPV_AMD_shader_explicit_vertex_parameter:
                options.emplace_back(L"-fspv-extension=SPV_AMD_shader_explicit_vertex_parameter");
                break;
            case ShaderVulkanExtension::SPV_GOOGLE_hlsl_functionality1:
                options.emplace_back(L"-fspv-extension=SPV_GOOGLE_hlsl_functionality1");
                break;
            case ShaderVulkanExtension::SPV_GOOGLE_user_type:
                options.emplace_back(L"-fspv-extension=SPV_GOOGLE_user_type");
                break;
            case ShaderVulkanExtension::SPV_NV_ray_tracing:
                options.emplace_back(L"-fspv-extension=SPV_NV_ray_tracing");
                break;
            case ShaderVulkanExtension::SPV_NV_mesh_shader:
                options.emplace_back(L"-fspv-extension=SPV_NV_mesh_shader");
                break;
            case ShaderVulkanExtension::SPV_KHR_ray_query:
                options.emplace_back(L"-fspv-extension=SPV_KHR_ray_query");
                break;
            case ShaderVulkanExtension::SPV_EXT_shader_image_int64:
                options.emplace_back(L"-fspv-extension=SPV_EXT_shader_image_int64");
                break;
            case ShaderVulkanExtension::SPV_KHR_physical_storage_buffer:
                options.emplace_back(L"-fspv-extension=SPV_KHR_physical_storage_buffer");
                break;
            case ShaderVulkanExtension::SPV_KHR_vulkan_memory_model:
                options.emplace_back(L"-fspv-extension=SPV_KHR_vulkan_memory_model");
                break;
            }
        }
    }

    /// <summary>
    /// Add SPIRV layout for the shader compiler.
    /// </summary>
    static void AddSpirvLayout(
        const ShaderCompileDesc&     desc,
        std::vector<WideString>&      registerShift,
        std::vector<const WideChar*>& options)
    {
        switch (desc.VulkanMemoryLayout)
        {
        case ShaderVulkanMemLayout::Dx:
            options.emplace_back(L"-fvk-use-dx-layout");
            break;
        case ShaderVulkanMemLayout::Gl:
            options.emplace_back(L"-fvk-use-gl-layout");
            break;
        case ShaderVulkanMemLayout::Scalar:
            options.emplace_back(L"-fvk-use-scalar-layout");
            break;
        }

        registerShift.emplace_back(std::format(L"{}", c_DefaultSpirvBindingOffset.textureOffset));
        registerShift.emplace_back(std::format(L"{}", c_DefaultSpirvBindingOffset.storageTextureAndBufferOffset));
        registerShift.emplace_back(std::format(L"{}", c_DefaultSpirvBindingOffset.samplerOffset));
        registerShift.emplace_back(std::format(L"{}", c_DefaultSpirvBindingOffset.constantBufferOffset));

        const WideChar* tShift = registerShift[0].c_str();
        const WideChar* UShift = registerShift[1].c_str();
        const WideChar* sShift = registerShift[2].c_str();
        const WideChar* bShift = registerShift[3].c_str();

        std::array spaces{
            L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7",
            L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15"
        };

        for (const WideChar* space : spaces)
        {
            options.emplace_back(L"-fvk-t-shift");
            options.emplace_back(tShift);
            options.emplace_back(space);

            options.emplace_back(L"-fvk-u-shift");
            options.emplace_back(UShift);
            options.emplace_back(space);

            options.emplace_back(L"-fvk-s-shift");
            options.emplace_back(sShift);
            options.emplace_back(space);

            options.emplace_back(L"-fvk-b-shift");
            options.emplace_back(bShift);
            options.emplace_back(space);
        }
    }

    //

    CompileShaderOption::CompileShaderOption(
        const ShaderResolveDesc& resolver,
        const ShaderCompileDesc& desc) :
        Api(resolver.DeviceDesc.get().graphicsAPI),
        TargetProfile(GetShaderTargetProfile(desc.Stage, desc.Profile)),
        EntryPoint(desc.EntryPoint)
    {
        using namespace EnumBitOperators;

        //

        auto& rhiDesc = resolver.DeviceDesc.get();

        FinalOptions = {
            DXC_ARG_ALL_RESOURCES_BOUND,
            L"-HV 2021",
            L"-E", EntryPoint,
            L"-T", TargetProfile.c_str()
        };

        if (!desc.RequiresShaderValidation())
        {
            FinalOptions.emplace_back(DXC_ARG_SKIP_VALIDATION);
        }

        if (rhiDesc.isDrawParametersEmulationEnabled)
        {
            FinalOptions.emplace_back(L"-DAME_ENABLE_DRAW_PARAMETERS_EMULATION=1");
        }

#ifdef AME_DIST
        FinalOptions.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#else
        if ((desc.Flags & ShaderCompileFlags::OptimizationLevel3) != ShaderCompileFlags::None)
        {
            FinalOptions.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);
        }
        else if ((desc.Flags & ShaderCompileFlags::OptimizationLevel2) != ShaderCompileFlags::None)
        {
            FinalOptions.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL2);
        }
        else if ((desc.Flags & ShaderCompileFlags::OptimizationLevel1) != ShaderCompileFlags::None)
        {
            FinalOptions.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL1);
        }
        else if ((desc.Flags & ShaderCompileFlags::OptimizationLevel0) != ShaderCompileFlags::None)
        {
            FinalOptions.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL0);
        }
#endif

        if ((desc.Flags & ShaderCompileFlags::RowMajor) != ShaderCompileFlags::None)
        {
            FinalOptions.emplace_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);
        }

        if ((desc.Flags & ShaderCompileFlags::TreatWarningsAsErrors) != ShaderCompileFlags::None)
        {
            FinalOptions.emplace_back(DXC_ARG_WARNINGS_ARE_ERRORS);
        }

#ifndef AME_DIST
        using namespace EnumBitOperators;

        if ((desc.Flags & ShaderCompileFlags::Debug) != ShaderCompileFlags::None)
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
        case nri::GraphicsAPI::D3D12:
        {
            FinalOptions.emplace_back(L"-DAME_SHADER_COMPILER_D3D12=1");
            break;
        }
        case nri::GraphicsAPI::VULKAN:
        {
            FinalOptions.emplace_back(L"-DAME_SHADER_COMPILER_SPIRV=1");
            FinalOptions.emplace_back(L"-spirv");

            AddSpirvExtensions(desc.SpirvExtensions, FinalOptions);
            AddSpirvLayout(desc, m_RegisterShift, FinalOptions);

            break;
        }
        }

        m_MacrosCombined.reserve(desc.Defines.size());
        FinalOptions.reserve(FinalOptions.size() + desc.Defines.size());

        for (auto& [Key, Value] : desc.Defines)
        {
            m_MacrosCombined.emplace_back(std::format(
                L"-D{}={}",
                Key,
                Value.empty() ? L"1" : Value.data()));
        }

        for (auto& Macro : m_MacrosCombined)
        {
            FinalOptions.emplace_back(Macro.c_str());
        }
    }
} // namespace Ame::Rhi