#include <Gfx/RG/Passes/GeometryPass.hpp>
#include <Gfx/RG/Resources/Names.hpp>
#include <Gfx/RG/ResourceStorage.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Gfx::RG::Std
{
    GeometryPass::GeometryPass(
        Ecs::Universe& Universe) :
        m_Universe(Universe)
    {
        Name("GeometryPass")
            .SetFlags(PassFlags::Graphics)
            .Build(
                [](Resolver& RgResolver)
                {
                    auto& Backbuffer = RgResolver.GetDevice().GetBackBufferDesc();
                    auto& FrameData  = RgResolver.GetFrameResourceData();

                    auto Desc      = Rhi::Tex2D(Backbuffer.format, FrameData.Viewport.x, FrameData.Viewport.y);
                    Desc.sampleNum = Backbuffer.sampleNum;

                    // ResourceId OutputImage("GeometryPass::OutputImage");
                    ResourceId OutputImage = Names::OutputImage;

                    RgResolver.CreateTexture(OutputImage, Desc);
                    RgResolver.WriteRenderTarget(
                        OutputImage("Main"),
                        Rhi::ShaderBits::DRAW,
                        RtvCustomDesc{
                            .ClearColor = { 0.0f, 0.0f, 1.0f, 1.0f },
                            .ClearType  = RG::ERTClearType::Color,
                            .ForceColor = true },
                        Desc.format);
                })
            .Execute(
                [this](const ResourceStorage& RgStorage, Rhi::CommandList* CommandList)
                {
                });
    }
} // namespace Ame::Gfx::RG::Std