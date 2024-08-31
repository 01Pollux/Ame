#include <CommonStates/CommonRenderPasses/BlitRenderPass.hpp>
#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Rhi
{
    void BlitRenderPass::Blit(const BlitCopyParameters& parameters)
    {
        auto deviceContext = m_RhiDevice->GetImmediateContext();
        Blit(deviceContext, parameters);
    }

    void BlitRenderPass::Blit(Dg::IDeviceContext* deviceContext, const BlitCopyParameters& parameters)
    {
        deviceContext->CopyTexture(parameters);
    }
} // namespace Ame::Rhi