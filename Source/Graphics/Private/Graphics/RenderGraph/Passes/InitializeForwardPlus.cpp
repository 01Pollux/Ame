#include <Graphics/RenderGraph/Passes/InitializeForwardPlus.hpp>

#include <Graphics/RenderGraph/Common/Names.hpp>

namespace Ame::Gfx
{
    InitializeForwardPlusPass::InitializeForwardPlusPass()
    {
        Name("Initialize Forward+ Pass")
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&InitializeForwardPlusPass::Build, this));
    }

    void InitializeForwardPlusPass::Build(
        RG::Resolver& resolver)
    {
        RG::RenderTargetViewDesc rtViewDesc{
            Dg::TextureViewDesc{
                "Forward+ RT",
                Dg::TEXTURE_VIEW_RENDER_TARGET,
                Dg::RESOURCE_DIM_TEX_2D },
            RG::RtvCustomDesc{
                .ClearColor = Colors::c_Snow,
                .ClearType  = RG::ERTClearType::Color,
                .ForceColor = true }
        };

        resolver.CreateTexture(c_RGFinalImage, resolver.GetBackbufferDesc());
        resolver.WriteTexture(c_RGFinalImage("Initialize"), Dg::BIND_RENDER_TARGET, rtViewDesc);
    }
} // namespace Ame::Gfx