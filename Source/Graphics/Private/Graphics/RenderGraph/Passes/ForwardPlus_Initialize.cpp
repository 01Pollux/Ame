#include <Graphics/RenderGraph/Passes/ForwardPlus_Initialize.hpp>

namespace Ame::Gfx
{
    ForwardPlus_InitializePass::ForwardPlus_InitializePass()
    {
        Name("Initialize Forward+ Pass")
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&ForwardPlus_InitializePass::Build, this));
    }

    void ForwardPlus_InitializePass::Build(
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

        resolver.CreateTexture(RGFinalImage.GetResource(), textureDesc);
        resolver.WriteTexture(RGFinalImage, Dg::BIND_RENDER_TARGET, rtv);
    }
} // namespace Ame::Gfx