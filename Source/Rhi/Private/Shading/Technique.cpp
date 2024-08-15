#include <boost/range/join.hpp>
#include <Rhi/Device/RhiDevice.hpp>

#include <Shading/Technique.hpp>
#include <Shading/Material.hpp>
#include <Shading/Hash.hpp>

#include <Rhi/Utils/DeviceWithCache.hpp>
#include <Rhi/Utils/PartialShader.hpp>

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
        const Material*                      material) const -> ShadersToKeepAliveList
    {
        using namespace EnumBitOperators;
        auto& renderStateShaders = m_RenderState.Links;
        auto& materialDesc       = material->GetMaterialDesc();

        ShadersToKeepAliveList shadersToKeepAlive;
        CombinedShader         shaderComposer;
        PartialShaderDesc      partialShaders[3]; // Prologue, Body, Epilogue
        uint32_t               partialShaderCount = 0;

        //

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

        RenderDeviceWithCache<false> renderDevice(m_RenderDevice);
        for (auto shaderType : MaterialCommonState::c_AllSupportedShaders)
        {
            Ptr<Dg::IShader> shader;

            auto iter = renderStateShaders.Shaders.find(shaderType);
            // No linked shader was set, so compile them from the source code
            if (iter == renderStateShaders.Shaders.end())
            {
                auto mainIter = renderStateShaders.Sources.find(shaderType);
                if (mainIter == renderStateShaders.Sources.end())
                {
                    continue;
                }

                partialShaderCount = 0;
                auto  preIter      = materialDesc.PreShaders.find(shaderType);
                auto  postIter     = materialDesc.PostShaders.find(shaderType);
                auto& mainShader   = mainIter->second;

                if (preIter != materialDesc.PreShaders.end())
                {
                    partialShaders[partialShaderCount++] = { &preIter->second.GetCreateInfo() };
                }
                if (postIter != materialDesc.PostShaders.end())
                {
                    partialShaders[partialShaderCount++] = { &postIter->second.GetCreateInfo() };
                }
                partialShaders[partialShaderCount++] = { &mainIter->second.GetCreateInfo() };

                CombinedShaderCreateDesc combinedDesc{ { partialShaders, partialShaderCount } };
                shaderComposer.Initialize(combinedDesc);

                shaderComposer
                    .ShaderType(shaderType)
                    .ShaderType(mainShader.ShaderType())
                    .SourceLanguage(mainShader.SourceLanguage())
                    .ShaderCompiler(mainShader.ShaderCompiler())
                    .HLSLVersion(mainShader.HLSLVersion())
                    .GLSLVersion(mainShader.GLSLVersion())
                    .GLESSLVersion(mainShader.GLESSLVersion())
                    .MSLVersion(mainShader.MSLVersion())
                    .WebGPUEmulatedArrayIndexSuffixCStr(mainShader.WebGPUEmulatedArrayIndexSuffix());

#ifndef AME_DIST
                shaderComposer.Name(std::format("{}_{:X}_{}", m_RenderState.Name, material->GetMaterialHash(), Dg::GetShaderTypeLiteralName(shaderType)));
#endif

                shader = renderDevice.CreateShader(shaderComposer.GetCreateInfo());

#ifndef AME_DIST
                if (!shader)
                {
                    Log::Gfx().Error("Failed to create shader for type: {}", Dg::GetShaderTypeLiteralName(shaderType));
                }
#endif

                shadersToKeepAlive.emplace_back(shader);
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
        const Material*                      material) const
    {
        auto& materialDesc = material->GetMaterialDesc();

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
        MaterialVertexInputLayout vertexInputLayout(vertexDesc.Flags);

        Dg::GraphicsPipelineStateCreateInfo psoCreateDesc;
        InitializePipelineState(psoCreateDesc, vertexDesc, material);
        psoCreateDesc.GraphicsPipeline.InputLayout = vertexInputLayout;

#ifndef AME_DIST
        String pipelineStateName   = std::format("{}_{}", m_RenderState.Name, material->GetName());
        psoCreateDesc.PSODesc.Name = pipelineStateName.c_str();
#endif

        auto signatures = CombineSignatures(psoCreateDesc, material);
        if (!signatures.empty())
        {
            psoCreateDesc.ppResourceSignatures = signatures.data();
        }

        auto shaders = CombineShaders(psoCreateDesc, material);

        RenderDeviceWithCache<false> renderDevice(m_RenderDevice);
        return renderDevice.CreateGraphicsPipelineState(psoCreateDesc);
    }
} // namespace Ame::Rhi