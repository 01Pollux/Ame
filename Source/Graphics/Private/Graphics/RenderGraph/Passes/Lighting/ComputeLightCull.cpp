#include <Graphics/RenderGraph/Passes/Lighting/ComputeLightCull.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>

#include <Rhi/Utils/DeviceWithCache.hpp>
#include <Shaders/Lighting/LightFrustumCull.hpp>

namespace Ame::Gfx
{
    ComputeLightCullPass::ComputeLightCullPass(uint8_t blockSize, uint16_t maxLightChunkSize) :
        m_BlockSize(blockSize), m_MaxLightChunkSize(maxLightChunkSize)
    {
        Name("Light Frustum Cull")
            .Build(std::bind_front(&ComputeLightCullPass::OnBuild, this))
            .Execute(std::bind_front(&ComputeLightCullPass::OnExecute, this));
    }

    //

    void ComputeLightCullPass::UpdateAndBindResourcesOnce(const Rg::ResourceStorage& storage,
                                                          Dg::IDeviceContext*        deviceContext)
    {
        auto lightIndices_Transparent = storage.GetBufferView(m_PassData.LightIndices_Transparent);
        auto lightIndices_Opaque      = storage.GetBufferView(m_PassData.LightIndices_Opaque);

        const uint32_t initParams[]{ m_BlockSize, 0 };
        deviceContext->UpdateBuffer(lightIndices_Transparent->GetBuffer(), 0, sizeof(initParams), initParams,
                                    Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        deviceContext->UpdateBuffer(lightIndices_Opaque->GetBuffer(), 0, sizeof(initParams), initParams,
                                    Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    void ComputeLightCullPass::CreateResourcesOnce(const Rg::ResourceStorage& storage)
    {
        if (m_PassData)
        {
            return;
        }

        {
            m_PassData.Srbs[0] = storage.GetUserData<Dg::IShaderResourceBinding>(
                c_RGFrameDataResourceSignature_Compute, Dg::IID_ShaderResourceBinding);
            m_PassData.Srbs[1] = storage.GetUserData<Dg::IShaderResourceBinding>(
                c_RGEntityResourceSignature_Compute, Dg::IID_ShaderResourceBinding);
        }

        if (!m_PipelineState)
        {
            Rhi::RenderDeviceWithCache<> device(storage.GetDevice());

            Dg::ComputePipelineStateCreateInfo computeDesc{
#ifndef AME_DIST
                "LightFrustumCullPso"
#endif
            };

            Ptr shader = device.CreateShader(
                Rhi::LightFrustumCull_ComputeShader(m_BlockSize, m_MaxLightChunkSize).GetCreateInfo());

            constexpr std::array resources{
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_COMPUTE, c_DepthView, Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_COMPUTE, c_LightIndices, Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_COMPUTE, c_LightIndices_Transparent,
                                          Dg::SHADER_RESOURCE_TYPE_BUFFER_UAV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_COMPUTE, c_LightIndices_Opaque,
                                          Dg::SHADER_RESOURCE_TYPE_BUFFER_UAV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_COMPUTE, c_LightHeads_Transparent,
                                          Dg::SHADER_RESOURCE_TYPE_TEXTURE_UAV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_COMPUTE, c_LightHeads_Opaque,
                                          Dg::SHADER_RESOURCE_TYPE_TEXTURE_UAV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
#ifndef AME_DIST
                Dg::PipelineResourceDesc{ Dg::SHADER_TYPE_COMPUTE, c_LightDebugTexture,
                                          Dg::SHADER_RESOURCE_TYPE_TEXTURE_UAV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
#endif
            };
            auto localSignature = device.CreatePipelineResourceSignature(
                { .Resources = resources.data(), .NumResources = Rhi::Count32(resources) });
            localSignature->CreateShaderResourceBinding(&m_Srb);

            std::array signatures{ localSignature.RawPtr(), m_PassData.Srbs[0]->GetPipelineResourceSignature(),
                                   m_PassData.Srbs[1]->GetPipelineResourceSignature() };
            computeDesc.pCS = shader;

            computeDesc.ppResourceSignatures    = signatures.data();
            computeDesc.ResourceSignaturesCount = Rhi::Count32(signatures);

            m_PipelineState = device.CreatePipelineState(computeDesc);
        }

        {
            auto lightIndices_Transparent = storage.GetBufferView(m_PassData.LightIndices_Transparent);
            auto lightIndices_Opaque      = storage.GetBufferView(m_PassData.LightIndices_Opaque);

            auto lightId   = storage.GetBufferView(m_PassData.LightIds);
            auto depthView = storage.GetTextureView(m_PassData.DepthView);

            auto lightHeads_Transparent = storage.GetTextureView(m_PassData.LightHeads_Transparent);
            auto lightHeads_Opaque      = storage.GetTextureView(m_PassData.LightHeads_Opaque);

            m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightIndices)->Set(lightId);
            // TODO: remove this check we finish implmenting the light cull cs
            m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_DepthView)->Set(depthView);

            m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightIndices_Transparent)
                ->Set(lightIndices_Transparent);
            m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightIndices_Opaque)->Set(lightIndices_Opaque);

            m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightHeads_Transparent)->Set(lightHeads_Transparent);
            m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightHeads_Opaque)->Set(lightHeads_Opaque);

#ifndef AME_DIST
            auto debugTexture = storage.GetTextureView(m_PassData.DebugTexture);
            m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightDebugTexture)->Set(debugTexture);
#endif
        }
    }

    //

    void ComputeLightCullPass::OnBuild(Rg::Resolver& resolver)
    {
        auto& textureDesc = resolver.GetBackbufferDesc();
        m_DispatchSize    = { static_cast<uint32_t>(std::ceil(static_cast<float>(textureDesc.Width) / m_BlockSize)),
                              static_cast<uint32_t>(std::ceil(static_cast<float>(textureDesc.Height) / m_BlockSize)) };
        uint32_t bufferSize =
            (m_DispatchSize.x() * m_DispatchSize.y() * c_AverageOverlappingLightsPerTile + 2) * sizeof(uint32_t);

        //

        Dg::BufferDesc bufferDesc{
            nullptr,
            bufferSize,
            Dg::BIND_SHADER_RESOURCE | Dg::BIND_UNORDERED_ACCESS,
        };
        bufferDesc.Mode              = Dg::BUFFER_MODE_STRUCTURED;
        bufferDesc.ElementByteStride = sizeof(uint32_t);

#ifndef AME_DIST
        bufferDesc.Name = "LightIndices_Transparent";
#endif
        resolver.CreateBuffer(c_RGLightIndices_Transparent, nullptr, bufferDesc);

#ifndef AME_DIST
        bufferDesc.Name = "LightIndices_Opaque";
#endif
        resolver.CreateBuffer(c_RGLightIndices_Opaque, nullptr, bufferDesc);

        //

        Dg::TextureDesc lightTextureDesc{
            nullptr, Dg::RESOURCE_DIM_TEX_2D, m_DispatchSize.x(), m_DispatchSize.y(), 1, Dg::TEX_FORMAT_RG32_UINT,
        };
        lightTextureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_UNORDERED_ACCESS;

#ifndef AME_DIST
        lightTextureDesc.Name = "LightHeads_Transparent";
#endif
        resolver.CreateTexture(c_RGLightHeads_Transparent, nullptr, lightTextureDesc);

#ifndef AME_DIST
        lightTextureDesc.Name = "LightHeads_Opaque";
#endif
        resolver.CreateTexture(c_RGLightHeads_Opaque, nullptr, lightTextureDesc);

#ifndef AME_DIST
        auto debugTextureDesc      = textureDesc;
        debugTextureDesc.Name      = "LightDebugTextures";
        debugTextureDesc.Format    = Dg::TEX_FORMAT_RGBA8_UNORM;
        debugTextureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_UNORDERED_ACCESS;

        resolver.CreateTexture(c_RGLightDebugTextures, nullptr, debugTextureDesc);
        m_PassData.DebugTexture =
            resolver.WriteTexture(c_RGLightDebugTextures, Dg::BIND_UNORDERED_ACCESS, Dg::TEXTURE_VIEW_UNORDERED_ACCESS);
#endif

        //

        m_PassData.LightIds =
            resolver.ReadBuffer(c_RGLightIdInstanceTable, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);
        m_PassData.DepthView =
            resolver.ReadTexture(c_RGDepthImage, Dg::BIND_SHADER_RESOURCE, Dg::TEXTURE_VIEW_SHADER_RESOURCE);

        m_PassData.LightIndices_Transparent = resolver.WriteBuffer(
            c_RGLightIndices_Transparent, Dg::BIND_UNORDERED_ACCESS, Dg::BUFFER_VIEW_UNORDERED_ACCESS);
        m_PassData.LightIndices_Opaque =
            resolver.WriteBuffer(c_RGLightIndices_Opaque, Dg::BIND_UNORDERED_ACCESS, Dg::BUFFER_VIEW_UNORDERED_ACCESS);

        m_PassData.LightHeads_Transparent = resolver.WriteTexture(
            c_RGLightHeads_Transparent, Dg::BIND_UNORDERED_ACCESS, Dg::TEXTURE_VIEW_UNORDERED_ACCESS);
        m_PassData.LightHeads_Opaque =
            resolver.WriteTexture(c_RGLightHeads_Opaque, Dg::BIND_UNORDERED_ACCESS, Dg::TEXTURE_VIEW_UNORDERED_ACCESS);

        resolver.ReadUserData(c_RGFrameDataResourceSignature_Compute);
        resolver.ReadUserData(c_RGEntityResourceSignature_Compute);

        //

        m_PassData.Reset();
    }

    void ComputeLightCullPass::OnExecute(const Rg::ResourceStorage& storage, Dg::IDeviceContext* deviceContext)
    {
        CreateResourcesOnce(storage);
        UpdateAndBindResourcesOnce(storage, deviceContext);

        //

        deviceContext->SetPipelineState(m_PipelineState);
        for (auto srb : m_PassData.Srbs)
        {
            deviceContext->CommitShaderResources(srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }
        deviceContext->CommitShaderResources(m_Srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Dg::DispatchComputeAttribs attrib{ m_DispatchSize.x(), m_DispatchSize.y(), 1 };
        deviceContext->DispatchCompute(attrib);
    }
} // namespace Ame::Gfx