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

    void ComputeLightCullPass::TryCreateResources(
        Rhi::IRhiDevice* rhiDevice)
    {
        if (m_PipelineState)
        {
            return;
        }

        Rhi::RenderDeviceWithCache<> device(rhiDevice);

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

        //

        m_LightIndices = m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "LightIndices");
        m_DepthTexture = m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "DepthTexture");

        m_LightIndices_Transparent = m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "LightIndices_Transparent");
        m_LightIndices_Opaque      = m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "LightIndices_Opaque");

        m_LightTexture_Transparent = m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "LightTexture_Transparent");
        m_LightTexture_Opaque      = m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "LightTexture_Opaque");

#ifndef AME_DIST
        m_DebugTexture = m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "DebugTexture");
#endif
    }

    //

    Co::result<void> ComputeLightCullPass::OnBuild(
        Rg::Resolver& resolver)
    {
        co_await resolver.ReadUserData(c_RGEntityResourceSignature_Compute);

        m_PassData.LightIds = co_await resolver.ReadBuffer(c_RGLightIdInstanceTable, Dg::BUFFER_VIEW_SHADER_RESOURCE);
    }

    void ComputeLightCullPass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        TryCreateResources(storage.GetDevice());

        //

        auto ersSrb = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Compute, Dg::IID_ShaderResourceBinding);

        //

        m_LightIndices->Set(m_PassData.LightIds);
        // m_DepthTexture->Set(storage.GetTextureView(c_RGDepthTexture).get().View);

        //

        deviceContext->SetPipelineState(m_PipelineState);
        deviceContext->CommitShaderResources(ersSrb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        deviceContext->CommitShaderResources(m_Srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
} // namespace Ame::Gfx