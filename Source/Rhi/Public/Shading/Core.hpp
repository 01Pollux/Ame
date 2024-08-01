#pragma once

#include <map>

#include <Core/Ame.hpp>
#include <Core/String.hpp>

#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    class Material;

    //

    enum class MaterialMode : uint8_t
    {
        Opaque,
        Transparent,
        Overlay
    };

    //

    struct MaterialShaderDesc
    {
        String                                 PreShaderCode;
        String                                 PostShaderCode;
        std::vector<std::pair<String, String>> Macros;
    };

    struct MaterialLinkShaderDesc
    {
        String                                 ShaderCode;
        std::vector<std::pair<String, String>> Macros;
    };

    using MaterialShaderSourceStorage     = std::map<Dg::SHADER_TYPE, MaterialShaderDesc>;
    using MaterialLinkShaderSourceStorage = std::map<Dg::SHADER_TYPE, MaterialLinkShaderDesc>;
    using MaterialShaderStorage           = std::map<Dg::SHADER_TYPE, Ptr<Dg::IShader>>;

    //

    struct MaterialDesc
    {
        String Name;

        size_t UserDataSize = 0;

        Dg::BlendStateDesc        Blend;
        Dg::RasterizerStateDesc   Rasterizer;
        Dg::DepthStencilStateDesc DepthStencil;

        /// 32-bit sample mask that determines which samples get updated
        /// in all the active render targets. A sample mask is always applied;
        /// it is independent of whether multisampling is enabled, and does not
        /// depend on whether an application uses multisample render targets.
        uint32_t SampleMask = 0xFFFF'FFFF;

        MaterialMode Mode;

        /// <summary>
        /// All shader sources must be written in HLSL
        /// </summary>
        MaterialShaderSourceStorage ShaderSources;
    };

    struct MaterialCreateDesc
    {
        MaterialDesc                      Desc;
        Dg::PipelineResourceSignatureDesc Properties{
            .BindingIndex = 2
        };
    };

    //

    /// <summary>
    /// We will merge both MaterialDesc::ShaderSources and MaterialShaderLinks::CreateSources to generate the shaders at runtime
    /// We use this to separate shader permutation between different pipelines such as deferred, forward, etc.
    /// </summary>
    struct MaterialShaderLinks
    {
        MaterialLinkShaderSourceStorage ShaderSources;
        MaterialShaderStorage           Shaders;
    };

    struct MaterialRenderState
    {
        String Name;

        /// <summary>
        /// Shader links that specify which shader will be used with this pipeline.
        /// </summary>
        MaterialShaderLinks Links;

        /// <summary>
        /// The resource signature that will be used with this pipeline.
        /// </summary>
        std::vector<Ptr<Dg::IPipelineResourceSignature>> Signatures;

        std::vector<Dg::TEXTURE_FORMAT> RenderTargets;
        Dg::SampleDesc                  Sample;

        /// Shading rate flags that specify which type of the shading rate will be used with this pipeline.
        Dg::PIPELINE_SHADING_RATE_FLAGS ShadingRateFlags = Dg::PIPELINE_SHADING_RATE_FLAG_NONE;

        Dg::TEXTURE_FORMAT DepthStencil = Dg::TEX_FORMAT_UNKNOWN;
        bool               ReadOnlyDSV  = false;

        Dg::PRIMITIVE_TOPOLOGY Topology = Dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    };

    //

    using MaterialHash = size_t;

    /// <summary>
    /// By default, we will have local space position for any given material
    /// </summary>
    enum class MaterialVertexInputFlags : uint16_t
    {
        Neon = 0,

        Position = 1 << 0, // VIX_Position
        Normal   = 1 << 1, // VIX_Normal
        TexCoord = 1 << 2, // VI_TexCoord
        Tangent  = 1 << 3, // VIX_Tangent

        Count = 7,

        Is2D = 1 << 8,
    };
} // namespace Ame::Rhi
