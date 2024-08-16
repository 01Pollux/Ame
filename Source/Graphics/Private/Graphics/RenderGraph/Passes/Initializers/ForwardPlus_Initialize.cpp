#include <Graphics/RenderGraph/Passes/Initializers/ForwardPlus_Initialize.hpp>

namespace Ame::Gfx
{
    ForwardPlus_InitializePass::ForwardPlus_InitializePass()
    {
        Name("Initialize Forward+ Pass")
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&ForwardPlus_InitializePass::OnBuild, this));
    }

    void ForwardPlus_InitializePass::OnBuild(
        RG::Resolver& resolver)
    {
        RG::RenderTargetViewDesc rtv{
            {},
            RG::RtvCustomDesc{
                .ClearColor = Colors::c_Snow,
                .ClearType  = RG::ERTClearType::Color,
                .ForceColor = true }
        };

        auto textureDesc = resolver.GetBackbufferDesc();
        textureDesc.BindFlags |= Dg::BIND_SHADER_RESOURCE;

        resolver.CreateTexture(c_RGFinalImage, textureDesc);
        resolver.WriteTexture(c_RGFinalImage("Initialize"), Dg::BIND_RENDER_TARGET, rtv);
    }
} // namespace Ame::Gfx