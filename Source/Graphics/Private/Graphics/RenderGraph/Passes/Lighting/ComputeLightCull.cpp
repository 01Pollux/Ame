#include <Graphics/RenderGraph/Passes/Lighting/ComputeLightCull.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>

#include <Rhi/Utils/DeviceWithCache.hpp>
#include <Shaders/Lighting/LightFrustumCull.hpp>

namespace Ame::Gfx
{
    ComputeLightCullPass::ComputeLightCullPass()
    {
        Name("Light Frustum Cull")
            .Flags(Rg::PassFlags::Compute)
            .Build(std::bind_front(&ComputeLightCullPass::OnBuild, this))
            .Execute(std::bind_front(&ComputeLightCullPass::OnExecute, this));
    }

    //

    void ComputeLightCullPass::BindResourcesOnce(
        const Rg::ResourceStorage& storage)
    {
        if (m_PassData)
        {
            return;
        }

        auto lightId = storage.GetBufferView(m_PassData.LightIds);

        auto lightIndices_Transparent = storage.GetBufferView(m_PassData.LightIndices_Transparent);
        auto lightIndices_Opaque      = storage.GetBufferView(m_PassData.LightIndices_Opaque);

        auto lightHeads_Transparent = storage.GetTextureView(m_PassData.LightHeads_Transparent);
        auto lightHeads_Opaque      = storage.GetTextureView(m_PassData.LightHeads_Opaque);

        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightIndices)->Set(lightId);
        // m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_DepthView)->Set(m_PassData.DepthView);

        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightIndices_Transparent)->Set(lightIndices_Transparent);
        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightIndices_Opaque)->Set(lightIndices_Opaque);

        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightHeads_Transparent)->Set(lightHeads_Transparent);
        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightHeads_Opaque)->Set(lightHeads_Opaque);

#ifndef AME_DIST
        auto debugTexture = storage.GetTextureView(m_PassData.DebugTexture);
        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, c_LightDebugTexture)->Set(debugTexture);
#endif

        m_PassData.Srbs[0] = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGFrameDataResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
        m_PassData.Srbs[1] = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
    }

    void ComputeLightCullPass::CreateResourcesOnce(
        const Rg::ResourceStorage& storage)
    {
        if (m_PipelineState)
        {
            return;
        }

        Rhi::RenderDeviceWithCache<> device(storage.GetDevice());

        Dg::ComputePipelineStateCreateInfo computeDesc{
#ifndef AME_DIST
            "LightFrustumCullPso"
#endif
        };
        computeDesc.PSODesc.ResourceLayout.DefaultVariableType = Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;

        Ptr shader = device.CreateShader(Rhi::LightFrustumCull_ComputeShader().GetCreateInfo());

        computeDesc.pCS = shader;
        m_PipelineState = device.CreatePipelineState(computeDesc);

        m_PipelineState->CreateShaderResourceBinding(&m_Srb);
    }

    //

    void ComputeLightCullPass::OnBuild(
        Rg::Resolver& resolver)
    {
        auto& textureDesc = resolver.GetBackbufferDesc();
        m_DispatchSize    = {
            static_cast<uint32_t>(std::ceil(static_cast<float>(textureDesc.Width) / m_BlockSize)),
            static_cast<uint32_t>(std::ceil(static_cast<float>(textureDesc.Height) / m_BlockSize))
        };
        uint32_t bufferSize = (m_DispatchSize.x() * m_DispatchSize.y() * c_AverageOverlappingLightsPerTile) * sizeof(uint32_t);

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
            nullptr,
            Dg::RESOURCE_DIM_TEX_2D,
            m_DispatchSize.x(),
            m_DispatchSize.y(),
            1,
            Dg::TEX_FORMAT_RG32_UINT,
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
        debugTextureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_UNORDERED_ACCESS;

        resolver.CreateTexture(c_RGLightDebugTextures, nullptr, debugTextureDesc);
        m_PassData.DebugTexture = resolver.WriteTexture(c_RGLightDebugTextures, Dg::BIND_UNORDERED_ACCESS, Dg::TEXTURE_VIEW_UNORDERED_ACCESS);
#endif

        //

        resolver.ReadUserData(c_RGEntityResourceSignature_Compute);

        m_PassData.LightIds  = resolver.ReadBuffer(c_RGLightIdInstanceTable, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);
        m_PassData.DepthView = resolver.ReadTexture(c_RGDepthImage, Dg::BIND_SHADER_RESOURCE, Dg::TEXTURE_VIEW_READ_ONLY_DEPTH_STENCIL);

        m_PassData.LightIndices_Transparent = resolver.WriteBuffer(c_RGLightIndices_Transparent, Dg::BIND_UNORDERED_ACCESS, Dg::BUFFER_VIEW_UNORDERED_ACCESS);
        m_PassData.LightIndices_Opaque      = resolver.WriteBuffer(c_RGLightIndices_Opaque, Dg::BIND_UNORDERED_ACCESS, Dg::BUFFER_VIEW_UNORDERED_ACCESS);

        m_PassData.LightHeads_Transparent = resolver.WriteTexture(c_RGLightHeads_Transparent, Dg::BIND_UNORDERED_ACCESS, Dg::TEXTURE_VIEW_UNORDERED_ACCESS);
        m_PassData.LightHeads_Opaque      = resolver.WriteTexture(c_RGLightHeads_Opaque, Dg::BIND_UNORDERED_ACCESS, Dg::TEXTURE_VIEW_UNORDERED_ACCESS);

        resolver.ReadUserData(c_RGFrameDataResourceSignature_Graphics);
        resolver.ReadUserData(c_RGEntityResourceSignature_Graphics);

        //

        m_PassData.Reset();
    }

    void ComputeLightCullPass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        CreateResourcesOnce(storage);
        BindResourcesOnce(storage);

        //

        deviceContext->SetPipelineState(m_PipelineState);
        for (auto srb : m_PassData.Srbs)
        {
            deviceContext->CommitShaderResources(srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }
        deviceContext->CommitShaderResources(m_Srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Dg::DispatchComputeAttribs attrib{
            m_DispatchSize.x(),
            m_DispatchSize.y(),
            1
        };
        deviceContext->DispatchCompute(attrib);
    }
} // namespace Ame::Gfx