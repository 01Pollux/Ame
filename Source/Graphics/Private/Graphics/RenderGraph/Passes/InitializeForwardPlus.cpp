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
        RG::RenderTargetViewDesc rtv{
            {},
            RG::RtvCustomDesc{
                .ClearColor = Colors::c_Snow,
                .ClearType  = RG::ERTClearType::Color,
                .ForceColor = true }
        };

        auto textureDesc      = resolver.GetBackbufferDesc();
        textureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_RENDER_TARGET;
        resolver.CreateTexture(c_RGFinalImage, textureDesc);
        resolver.WriteTexture(c_RGFinalImage("Initialize"), Dg::BIND_RENDER_TARGET, rtv);
    }
} // namespace Ame::Gfx