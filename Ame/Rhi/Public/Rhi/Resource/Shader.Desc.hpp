#pragma once

#include <Rhi/Resource/Shader.hpp>

namespace Ame::Asset
{
    class Storage;
} // namespace Ame::Asset

namespace Ame::Rhi
{
    enum class ShaderProfile : uint8_t
    {
        _6_1,
        _6_2,
        _6_3,
        _6_4,
        _6_5,
        _6_6,
    };

    enum class ShaderVulkanVersion : uint8_t
    {
        _1_0,
        _1_1,
        _1_2,
        _1_3,
    };

    enum class ShaderVulkanMemLayout : uint8_t
    {
        Dx,
        Gl,
        Scalar
    };

    enum class ShaderVulkanExtension : uint8_t
    {
        KHR,
        SPV_KHR_16bit_storage,
        SPV_KHR_device_group,
        SPV_KHR_fragment_shading_rate,
        SPV_KHR_non_semantic_info,
        SPV_KHR_multiview,
        SPV_KHR_shader_draw_parameters,
        SPV_KHR_post_depth_coverage,
        SPV_KHR_ray_tracing,
        SPV_KHR_shader_clock,
        SPV_EXT_demote_to_helper_invocation,
        SPV_EXT_descriptor_indexing,
        SPV_EXT_fragment_fully_covered,
        SPV_EXT_fragment_invocation_density,
        SPV_EXT_mesh_shader,
        SPV_EXT_shader_stencil_export,
        SPV_EXT_shader_viewport_index_layer,
        SPV_AMD_gpu_shader_half_float,
        SPV_AMD_shader_early_and_late_fragment_tests,
        SPV_AMD_shader_explicit_vertex_parameter,
        SPV_GOOGLE_hlsl_functionality1,
        SPV_GOOGLE_user_type,
        SPV_NV_ray_tracing,
        SPV_NV_mesh_shader,
        SPV_KHR_ray_query,
        SPV_EXT_shader_image_int64,
        SPV_KHR_physical_storage_buffer,
        SPV_KHR_vulkan_memory_model
    };

    enum class ShaderCompileFlags : uint32_t
    {
        None = 0,

        LibraryShader = 1 << 0,

        Debug                = 1 << 1,
        DisableOptimizations = 1 << 2,
        NoValidation         = 1 << 3,

        OptimizationLevel0 = 1 << 4,
        OptimizationLevel1 = 1 << 5,
        OptimizationLevel2 = 1 << 6,
        OptimizationLevel3 = 1 << 7,

        RowMajor              = 1 << 8,
        TreatWarningsAsErrors = 1 << 9
    };

    using ShaderMacroList = std::unordered_map<WideString, WideString>;

    struct ShaderCompileDesc
    {
        ShaderMacroList                    Defines;
        std::vector<ShaderVulkanExtension> SpirvExtensions{ ShaderVulkanExtension::KHR };

        ShaderType            Stage              = ShaderType::NONE;
        ShaderProfile         Profile            = ShaderProfile::_6_5;
        ShaderVulkanMemLayout VulkanMemoryLayout = ShaderVulkanMemLayout::Dx;

        ShaderCompileFlags Flags{
#ifdef AME_DEBUG
            ShaderCompileFlags::Debug
#endif
        };
    };
} // namespace Ame::Rhi