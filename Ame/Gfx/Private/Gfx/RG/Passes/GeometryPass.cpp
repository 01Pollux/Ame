#include <Gfx/RG/Passes/GeometryPass.hpp>
#include <Gfx/RG/ResourceStorage.hpp>
#include <Rhi/Device/Device.hpp>

#include <Gfx/Ecs/Component/Visibility.hpp>

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
                    ResourceId OutputImage = ResourceId::OutputImage;

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
                    CreateRenderRules();

                    auto& FrameData = RgStorage.GetFrameResourceData();

                    // auto& Batcher = RgStorage.GetSceneBatcher();
                    auto& Batcher = m_DrawBatcher;

                    auto CameraIndex = m_Render2DRule.find_var("Camera");

                    Ecs::Iterable RenderIter =
                        m_Render2DRule.iter()
                            .set_var(CameraIndex, FrameData.CurrentCamera);

                    RenderIter.iter([&](Ecs::Iterator& Iter, const Ecs::Gfx::Component::Renderable2D* Renderables) {
                        Batcher.
                    });
                });
    }

    void GeometryPass::CreateRenderRules()
    {
        auto ActiveWorld = m_Universe.get().GetActiveWorld();
        if (ActiveWorld == nullptr) [[unlikely]]
        {
            m_Render2DRule = {};
            return;
        }

        if (m_Render2DRule) [[likely]]
        {
            return;
        }

        m_Render2DRule = ActiveWorld
                             ->CreateRule<const Ecs::Gfx::Component::Renderable2D>()
                             .with<Ecs::Gfx::Component::VisibleBy>("$Camera")
                             .build();
    }
} // namespace Ame::Gfx::RG::Std