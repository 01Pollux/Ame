#include <CommonStates/CommonRenderPasses/BlitRenderPass.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Rhi
{
    void BlitRenderPass::Blit(
        const BlitDrawParameters& parameters)
    {
        auto deviceContext = m_RhiDevice->GetImmediateContext();
        Blit(deviceContext, parameters);
    }

    void BlitRenderPass::Blit(
        Dg::IDeviceContext*       deviceContext,
        const BlitDrawParameters& parameters)
    {
        auto& [pso, srb] = GetPipelineState(parameters);

        deviceContext->SetPipelineState(pso);

        Dg::ITextureView* rtvs[] = { parameters.TargetView };
        deviceContext->SetRenderTargets(std::size(rtvs), rtvs, nullptr, parameters.DstTranisiton);

        srb->GetVariableByName(Dg::SHADER_TYPE_PIXEL, "_Texture")->Set(parameters.SourceView);
        deviceContext->CommitShaderResources(srb, parameters.SrcTransition);

        deviceContext->Draw(Dg::DrawAttribs{ 4, Dg::DRAW_FLAGS::DRAW_FLAG_VERIFY_ALL });
    }
} // namespace Ame::Rhi