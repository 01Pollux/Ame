#include <Graphics/RenderGraph/Passes/Initializers/ForwardPlus_Initialize.hpp>

namespace Ame::Gfx
{
    ForwardPlus_InitializePass::ForwardPlus_InitializePass()
    {
        Name("Initialize Forward+ Pass")
            .Flags(Rg::PassFlags::Graphics)
            .Build(std::bind_front(&ForwardPlus_InitializePass::OnBuild, this));
    }

    void ForwardPlus_InitializePass::OnBuild(
        Rg::Resolver& resolver)
    {
        Rg::RenderTargetViewDesc rtv{
            {},
            Rg::RtvCustomDesc{
                .ClearColor = Colors::c_DarkGray,
                .ClearType  = Rg::ERTClearType::Color,
                .ForceColor = true }
        };

        auto textureDesc = resolver.GetBackbufferDesc();
        textureDesc.BindFlags |= Dg::BIND_SHADER_RESOURCE;

        resolver.CreateTexture(c_RGFinalImage, textureDesc);
        resolver.WriteTexture(c_RGFinalImage("Initialize"), Dg::BIND_RENDER_TARGET, rtv);
    }
} // namespace Ame::Gfx