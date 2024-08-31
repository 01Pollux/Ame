#include <Graphics/RenderGraph/Passes/Initializers/ForwardPlus_Initialize.hpp>

namespace Ame::Gfx
{
    ForwardPlus_InitializePass::ForwardPlus_InitializePass()
    {
        Name("Initialize Forward+ Pass")
            .Flags(Rg::PassFlags::Graphics)
            .Build(std::bind_front(&ForwardPlus_InitializePass::OnBuild, this))
            .Execute(std::bind_front(&ForwardPlus_InitializePass::OnExecute, this));
    }

    void ForwardPlus_InitializePass::OnBuild(Rg::Resolver& resolver)
    {
        Rg::TextureResourceDesc textureDesc{ resolver.GetBackbufferDesc() };
        textureDesc.BindFlags |= Dg::BIND_SHADER_RESOURCE;

        resolver.CreateTexture(c_RGFinalImage, nullptr, textureDesc);
        m_PassData.FinalImageViewId =
            resolver.WriteTexture(c_RGFinalImage, Dg::BIND_RENDER_TARGET, Dg::TEXTURE_VIEW_RENDER_TARGET);
    }

    void ForwardPlus_InitializePass::OnExecute(const Rg::ResourceStorage& storage, Dg::IDeviceContext* deviceContext)
    {
        auto rtv = storage.GetTextureView(m_PassData.FinalImageViewId);

        deviceContext->SetRenderTargets(1, &rtv, nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        deviceContext->ClearRenderTarget(rtv, Colors::c_DarkGray.data(), Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }
} // namespace Ame::Gfx