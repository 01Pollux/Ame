#pragma once

#include <map>

#include <Core/Ame.hpp>
#include <Core/String.hpp>

#include <Rhi/Core.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>
#include <Shading/VertexInput.hpp>

namespace Ame::Rhi
{
    class Material;
    class MaterialTechnique;

    static constexpr uint32_t c_MaterialBindingIndex = 0;

    //

    struct MaterialBaseShaderDesc
    {
        std::vector<std::pair<String, String>>                Macros;
        std::vector<Ptr<Dg::IShaderSourceInputStreamFactory>> StreamFactories;

        MaterialBaseShaderDesc() = default;

        MaterialBaseShaderDesc(
            const Dg::ShaderCreateInfo& createInfo) :
            StreamFactories({ Ptr{ createInfo.pShaderSourceStreamFactory } })
        {
            Macros.reserve(createInfo.Macros.Count);
            for (auto& element : std::span{ createInfo.Macros.Elements, createInfo.Macros.Count })
            {
                Macros.emplace_back(element.Name, element.Definition);
            }
        }
    };

    struct MaterialShaderDesc : MaterialBaseShaderDesc
    {
        String PreShaderCode;
        String PostShaderCode;

        MaterialShaderDesc() = default;

        MaterialShaderDesc(
            const Dg::ShaderCreateInfo& createInfo) :
            MaterialBaseShaderDesc(createInfo),
            PreShaderCode{ createInfo.Source, createInfo.SourceLength }
        {
        }

        MaterialShaderDesc(
            const Dg::ShaderCreateInfo& createInfo,
            const String&               postShaderCode) :
            MaterialBaseShaderDesc(createInfo),
            PreShaderCode{ createInfo.Source, createInfo.SourceLength },
            PostShaderCode{ postShaderCode }
        {
        }

        MaterialShaderDesc(
            const Dg::ShaderCreateInfo& createInfo,
            const String&               preShaderCode,
            const String&               postShaderCode) :
            MaterialBaseShaderDesc(createInfo),
            PreShaderCode{ preShaderCode },
            PostShaderCode{ postShaderCode }
        {
        }
    };

    struct MaterialLinkShaderDesc : MaterialBaseShaderDesc
    {
        String ShaderCode;

        MaterialLinkShaderDesc() = default;

        MaterialLinkShaderDesc(
            const Dg::ShaderCreateInfo& createInfo) :
            MaterialBaseShaderDesc(createInfo),
            ShaderCode{ createInfo.Source, createInfo.SourceLength }
        {
        }
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

    struct MaterialVertexDesc
    {
        Dg::PRIMITIVE_TOPOLOGY Topology = Dg::PRIMITIVE_TOPOLOGY_UNDEFINED;
        VertexInputFlags       Flags    = VertexInputFlags::None;
    };

    struct MaterialVertexInputLayout
    {
    public:
        MaterialVertexInputLayout() noexcept = default;
        MaterialVertexInputLayout(
            VertexInputFlags flags) noexcept
        {
            for (uint32_t i = 0; i < Rhi::Count32(c_InputVertexAttributes); i++)
            {
                bool hasAttribute           = (std::to_underlying(flags) & (1 << i)) != 0;
                m_Elements[i].InputIndex    = i;
                m_Elements[i].BufferSlot    = c_InputVertexAttributes[i].BufferId;
                m_Elements[i].NumComponents = c_InputVertexAttributes[i].NumComponents;
                m_Elements[i].ValueType     = c_InputVertexAttributes[i].Type;
                m_Elements[i].IsNormalized  = (m_Elements[i].ValueType == Dg::VT_UINT8 ||
                                              m_Elements[i].ValueType == Dg::VT_INT8);
                if (!hasAttribute)
                {
                    m_Elements[i].Stride = 0;
                }
            }
        }

        [[nodiscard]] operator Dg::InputLayoutDesc() const noexcept
        {
            return { m_Elements, Rhi::Count32(m_Elements) };
        }

    private:
        Dg::LayoutElement m_Elements[std::to_underlying(VertexInputFlags::Count)];
    };

    //

    using MaterialHash = size_t;
} // namespace Ame::Rhi
