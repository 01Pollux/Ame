#pragma once

#include <map>

#include <Core/Ame.hpp>
#include <Core/String.hpp>

#include <Rhi/Utils/ShaderCreateInfoX.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>

#include <Shading/VertexInput.hpp>
#include <Shading/PropertyDescriptor.hpp>

namespace Ame::Rhi
{
    class Material;
    class MaterialTechnique;

    //

    struct MaterialBaseShaderDesc
    {
        std::vector<std::pair<String, String>>                Macros;
        std::vector<Ptr<Dg::IShaderSourceInputStreamFactory>> StreamFactories;

        MaterialBaseShaderDesc() = default;

        MaterialBaseShaderDesc(const Dg::ShaderCreateInfo& createInfo) :
            StreamFactories({ Ptr{ createInfo.pShaderSourceStreamFactory } })
        {
            Macros.reserve(createInfo.Macros.Count);
            for (auto& element : std::span{ createInfo.Macros.Elements, createInfo.Macros.Count })
            {
                Macros.emplace_back(element.Name, element.Definition);
            }
        }
    };

    using MaterialShaderSourceStorage = std::map<Dg::SHADER_TYPE, ShaderCreateInfoX>;
    using MaterialShaderStorage       = std::map<Dg::SHADER_TYPE, Ptr<Dg::IShader>>;

    //

    struct MaterialCreateDesc
    {
        /// <summary>
        /// all shaders here must contain void post_main(...)
        /// </summary>
        MaterialShaderSourceStorage       Shaders;
        MaterialPropertyDescriptor        UserData;
        Dg::PipelineResourceSignatureDesc ResourceSignature;
    };

    //

    /// <summary>
    /// We will merge both MaterialCreateDesc::Shaders and MaterialShaderLinks::Sources to generate the shaders at
    /// runtime We use this to separate shader permutation between different pipelines such as deferred, forward, etc.
    /// </summary>
    struct MaterialShaderLinks
    {
        /// <summary>
        /// All shaders here must contain void main(...)
        /// </summary>
        MaterialShaderSourceStorage Sources;

        // Precompiled shaders
        MaterialShaderStorage Shaders;

        // Shaders to bind to the pso
        Dg::SHADER_TYPE ActiveShaders = Dg::SHADER_TYPE_ALL_GRAPHICS;
    };

    struct MaterialRenderState
    {
        String Name;

        Dg::BlendStateDesc Blend;

        Dg::RasterizerStateDesc Rasterizer = Dg::RS_SolidFillCullBack;

        Dg::DepthStencilStateDesc DepthStencil;

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

        /// 32-bit sample mask that determines which samples get updated
        /// in all the active render targets. A sample mask is always applied;
        /// it is independent of whether multisampling is enabled, and does not
        /// depend on whether an application uses multisample render targets.
        uint32_t SampleMask = 0xFFFF'FFFF;

        /// Shading rate flags that specify which type of the shading rate will be used with this pipeline.
        Dg::PIPELINE_SHADING_RATE_FLAGS ShadingRateFlags = Dg::PIPELINE_SHADING_RATE_FLAG_NONE;

        Dg::TEXTURE_FORMAT DSFormat    = Dg::TEX_FORMAT_UNKNOWN;
        bool               ReadOnlyDSV = false;
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
        MaterialVertexInputLayout(VertexInputFlags flags) noexcept
        {
            for (uint8_t i = 0; i < std::to_underlying(VertexInputFlags::Count); i++)
            {
                bool hasAttribute           = (std::to_underlying(flags) & (1 << i)) != 0;
                m_Elements[i].InputIndex    = i;
                m_Elements[i].BufferSlot    = i;
                m_Elements[i].NumComponents = c_InputVertexAttributes[i].NumComponents;
                m_Elements[i].ValueType     = c_InputVertexAttributes[i].Type;
                m_Elements[i].IsNormalized =
                    (m_Elements[i].ValueType == Dg::VT_UINT8 || m_Elements[i].ValueType == Dg::VT_INT8);
                if (!hasAttribute)
                {
                    m_Elements[i].Stride = 0;
                }
            }
        }

        [[nodiscard]] operator Dg::InputLayoutDesc() const noexcept
        {
            return { m_Elements, Count32(m_Elements) };
        }

    private:
        Dg::LayoutElement m_Elements[std::to_underlying(VertexInputFlags::Count)];
    };

    //

    using MaterialHash = size_t;
} // namespace Ame::Rhi
