#pragma once

#include <map>

#include <Core/Ame.hpp>
#include <Core/String.hpp>

#include <Rhi/Core.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>

namespace Ame::Rhi
{
    class Material;
    class MaterialTechnique;

    static constexpr uint32_t c_MaterialBindingIndex = 0;

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
        Dg::RasterizerStateDesc   Rasterizer   = Dg::RS_SolidFillCullBack;
        Dg::DepthStencilStateDesc DepthStencil = Dg::DSS_DisableDepth;

        /// 32-bit sample mask that determines which samples get updated
        /// in all the active render targets. A sample mask is always applied;
        /// it is independent of whether multisampling is enabled, and does not
        /// depend on whether an application uses multisample render targets.
        uint32_t SampleMask = 0xFFFF'FFFF;

        /// <summary>
        /// All shader sources must be written in HLSL
        /// </summary>
        MaterialShaderSourceStorage ShaderSources;
    };

    struct MaterialCreateDesc
    {
        MaterialDesc                      Desc;
        Dg::PipelineResourceSignatureDesc Properties{
            .BindingIndex = c_MaterialBindingIndex // must be c_MaterialBindingIndex
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
    };

    //

    using MaterialHash = size_t;

    enum class MaterialVertexInputTypes : uint16_t
    {
        Position, // VIX_Position
        Normal,   // VIX_Normal
        TexCoord, // VI_TexCoord
        Tangent,  // VIX_Tangent
        Count,
    };

    enum class MaterialVertexInputFlags : uint16_t
    {
        None     = 0,
        Position = 1 << std::to_underlying(MaterialVertexInputTypes::Position), // VIX_Position
        Normal   = 1 << std::to_underlying(MaterialVertexInputTypes::Normal),   // VIX_Normal
        TexCoord = 1 << std::to_underlying(MaterialVertexInputTypes::TexCoord), // VI_TexCoord
        Tangent  = 1 << std::to_underlying(MaterialVertexInputTypes::Tangent),  // VIX_Tangent
    };
} // namespace Ame::Rhi
