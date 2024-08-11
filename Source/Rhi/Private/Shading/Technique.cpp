#include <boost/range/join.hpp>
#include <Rhi/Device/RhiDevice.hpp>

#include <Shading/Technique.hpp>
#include <Shading/Material.hpp>
#include <Shading/ShaderComposer.hpp>
#include <Shading/Hash.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/ShaderSourceFactoryUtils.h>

#include <Core/Enum.hpp>
#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    MaterialTechnique* MaterialTechnique::Create(
        Dg::IRenderDevice*  renderDevice,
        MaterialRenderState renderState)
    {
        return ObjectAllocator<MaterialTechnique>()(renderDevice, std::move(renderState));
    }

    MaterialTechnique::MaterialTechnique(
        IReferenceCounters* counters,
        Dg::IRenderDevice*  renderDevice,
        MaterialRenderState renderState) :
        Base(counters),
        m_RenderDevice(renderDevice),
        m_RenderState(std::move(renderState))
    {
    }

    //

    Dg::IPipelineState* MaterialTechnique::GetPipelineState(
        const MaterialVertexDesc&    vertexDesc,
        const Material*              material,
        Dg::IShaderResourceBinding** srb) const
    {
        auto hash = Dg::ComputeHash(
            material->GetMaterialHash(),
            vertexDesc);

        auto& pipelineState = m_PipelineStates[hash];
        if (!pipelineState)
        {
            pipelineState = CreatePipelineState(vertexDesc, material);
        }
        if (srb)
        {
            auto& srbState = m_SRBs[hash];
            if (!srbState)
            {
                pipelineState->CreateShaderResourceBinding(&srbState);
            }
            *srb = srbState;
        }
        return pipelineState;
    }

    //

    auto MaterialTechnique::CombineShaders(
        Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
        const MaterialDesc&                  materialDesc) const -> ShadersToKeepAliveList
    {
        using namespace EnumBitOperators;

        auto setShader = [&graphicsPsoDesc](Dg::SHADER_TYPE type, Dg::IShader* shader)
        {
            switch (type)
            {
            case Dg::SHADER_TYPE_VERTEX:
                graphicsPsoDesc.pVS = shader;
                break;
            case Dg::SHADER_TYPE_PIXEL:
                graphicsPsoDesc.pPS = shader;
                break;
            case Dg::SHADER_TYPE_GEOMETRY:
                graphicsPsoDesc.pGS = shader;
                break;
            case Dg::SHADER_TYPE_HULL:
                graphicsPsoDesc.pHS = shader;
                break;
            case Dg::SHADER_TYPE_DOMAIN:
                graphicsPsoDesc.pDS = shader;
                break;
            case Dg::SHADER_TYPE_AMPLIFICATION:
                graphicsPsoDesc.pAS = shader;
                break;
            case Dg::SHADER_TYPE_MESH:
                graphicsPsoDesc.pMS = shader;
                break;
            default:
                break;
            }
        };

        //

        std::vector<Dg::ShaderMacro> macros;

        auto initializeShaderSource = [](auto iter, auto end, auto getter)
        {
            StringView sourceCode = "";
            if (iter != end)
            {
                auto& desc = iter->second;
                sourceCode = getter(desc);
            }
            return sourceCode;
        };

        auto initializeShaderMacros = [&macros](auto iter, auto end)
        {
            if (iter == end)
            {
                return;
            }

            auto& desc = iter->second;
            macros.reserve(macros.size() + desc.Macros.size());
            for (auto& [name, value] : desc.Macros)
            {
                StringView nameView(name);
                StringView valueView(value);
                macros.emplace_back(nameView.data(), valueView.data());
            }
        };

        auto initializeStreamFactory = [](auto lhs, auto lhsEnd, auto rhs, auto rhsEnd)
        {
            std::span<const Ptr<Dg::IShaderSourceInputStreamFactory>> lhsStream, rhsStream;
            if (lhs != lhsEnd)
            {
                lhsStream = lhs->second.StreamFactories;
            }
            if (rhs != rhsEnd)
            {
                rhsStream = rhs->second.StreamFactories;
            }

            auto streams = boost::range::join(lhsStream, rhsStream) |
                           std::views::transform([](auto& factory) -> Dg::IShaderSourceInputStreamFactory*
                                                 { return factory; });

            Ptr<Dg::IShaderSourceInputStreamFactory> factory;
            if (streams.size() > 1)
            {
                auto streamsList = streams |
                                   std::ranges::to<std::vector>();

                Dg::CreateCompoundShaderSourceFactory({ streamsList.data(), static_cast<uint32_t>(streamsList.size()) }, &factory);
            }
            else if (!streams.empty())
            {
                factory = streams.front();
            }
            return factory;
        };

        //

        auto& renderStateShaders = m_RenderState.Links;

        MaterialShaderComposer shaderComposer;
        Dg::ShaderCreateInfo   createDesc{ nullptr, nullptr, Dg::SHADER_SOURCE_LANGUAGE_HLSL };

        ShadersToKeepAliveList shadersToKeepAlive;

        for (auto shaderType : MaterialCommonState::c_AllSupportedShaders)
        {
            macros.clear();
            Ptr<Dg::IShader> shader;

            auto iter = renderStateShaders.Shaders.find(shaderType);
            // No linked shader was set, so compile them from the source code
            if (iter == renderStateShaders.Shaders.end())
            {
                auto materialIter = materialDesc.ShaderSources.find(shaderType);
                auto bodyIter     = renderStateShaders.ShaderSources.find(shaderType);

                // fetch source code
                StringView prologueSource = initializeShaderSource(
                    materialIter,
                    materialDesc.ShaderSources.end(),
                    [](auto& desc) -> StringView
                    { return desc.PreShaderCode; });

                StringView epilogueSource = initializeShaderSource(
                    materialIter,
                    materialDesc.ShaderSources.end(),
                    [](auto& desc) -> StringView
                    { return desc.PostShaderCode; });

                StringView bodySource = initializeShaderSource(
                    bodyIter,
                    renderStateShaders.ShaderSources.end(),
                    [](auto& desc) -> StringView
                    { return desc.ShaderCode; });

                // No shader was found, so skip it
                if (prologueSource.empty() &&
                    epilogueSource.empty() &&
                    bodySource.empty())
                {
                    continue;
                }

                initializeShaderMacros(materialIter, materialDesc.ShaderSources.end());
                initializeShaderMacros(bodyIter, renderStateShaders.ShaderSources.end());

                // Initialize macros
                createDesc.Macros.Elements = macros.data();
                createDesc.Macros.Count    = macros.size();
                createDesc.Desc.ShaderType = shaderType;

                auto sourceCode = shaderComposer.Write(prologueSource, bodySource, epilogueSource);
                auto factory    = initializeStreamFactory(materialIter, materialDesc.ShaderSources.end(), bodyIter, renderStateShaders.ShaderSources.end());

                createDesc.Source       = sourceCode.data();
                createDesc.SourceLength = sourceCode.size();

                createDesc.pShaderSourceStreamFactory = factory;

                m_RenderDevice->CreateShader(createDesc, &shader);

#ifndef AME_DIST
                if (!shader)
                {
                    Log::Gfx().Error("Failed to create shader for type: {}", Dg::GetShaderTypeLiteralName(shaderType));
                }
#endif

                shadersToKeepAlive.emplace_back(shader);
                macros.clear();
            }
            else
            {
                shader = iter->second;
            }

            setShader(shaderType, shader);
        }

        return shadersToKeepAlive;
    }

    auto MaterialTechnique::CombineSignatures(
        Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
        const Material*                      material) const -> SignaturesToKeepAlive
    {
        SignaturesToKeepAlive signatures;
        signatures.reserve(m_RenderState.Signatures.size() + 1);

        for (auto& signature : m_RenderState.Signatures)
        {
            signatures.push_back(signature);
        }
        if (auto signature = material->GetResourceSignature())
        {
            signatures.push_back(signature);
        }

        graphicsPsoDesc.ResourceSignaturesCount = Rhi::Count32(signatures);
        return signatures;
    }

    void MaterialTechnique::InitializePipelineState(
        Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
        const MaterialVertexDesc&            vertexDesc,
        const MaterialDesc&                  materialDesc) const
    {
        graphicsPsoDesc.PSODesc.ResourceLayout.DefaultVariableType        = Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;
        graphicsPsoDesc.PSODesc.ResourceLayout.DefaultVariableMergeStages = Dg::SHADER_TYPE_ALL_GRAPHICS;

        graphicsPsoDesc.GraphicsPipeline.BlendDesc         = materialDesc.Blend;
        graphicsPsoDesc.GraphicsPipeline.SampleMask        = materialDesc.SampleMask;
        graphicsPsoDesc.GraphicsPipeline.RasterizerDesc    = materialDesc.Rasterizer;
        graphicsPsoDesc.GraphicsPipeline.DepthStencilDesc  = materialDesc.DepthStencil;
        graphicsPsoDesc.GraphicsPipeline.PrimitiveTopology = vertexDesc.Topology;
        graphicsPsoDesc.GraphicsPipeline.NumRenderTargets  = Rhi::Count8(m_RenderState.RenderTargets);
        graphicsPsoDesc.GraphicsPipeline.ShadingRateFlags  = m_RenderState.ShadingRateFlags;

        for (size_t i = 0; i < m_RenderState.RenderTargets.size(); i++)
        {
            graphicsPsoDesc.GraphicsPipeline.RTVFormats[i] = m_RenderState.RenderTargets[i];
        }

        graphicsPsoDesc.GraphicsPipeline.DSVFormat   = m_RenderState.DepthStencil;
        graphicsPsoDesc.GraphicsPipeline.ReadOnlyDSV = m_RenderState.ReadOnlyDSV;
        graphicsPsoDesc.GraphicsPipeline.SmplDesc    = m_RenderState.Sample;
    }

    Ptr<Dg::IPipelineState> MaterialTechnique::CreatePipelineState(
        const MaterialVertexDesc& vertexDesc,
        const Material*           material) const
    {
        auto& materialDesc = material->GetMaterialDesc();

        MaterialVertexInputLayout vertexInputLayout(vertexDesc.Flags);

        Dg::GraphicsPipelineStateCreateInfo psoCreateDesc;
        InitializePipelineState(psoCreateDesc, vertexDesc, materialDesc);
        psoCreateDesc.GraphicsPipeline.InputLayout = vertexInputLayout;

#ifndef AME_DIST
        String pipelineStateName   = std::format("{}_{}", materialDesc.Name, m_RenderState.Name);
        psoCreateDesc.PSODesc.Name = pipelineStateName.c_str();
#endif

        auto signatures = CombineSignatures(psoCreateDesc, material);
        if (!signatures.empty())
        {
            psoCreateDesc.ppResourceSignatures = signatures.data();
        }

        auto shaders = CombineShaders(psoCreateDesc, materialDesc);

        Ptr<Dg::IPipelineState> pipelineState;
        m_RenderDevice->CreateGraphicsPipelineState(psoCreateDesc, &pipelineState);

        return pipelineState;
    }
} // namespace Ame::Rhi