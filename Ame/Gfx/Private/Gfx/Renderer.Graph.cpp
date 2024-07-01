#include <Gfx/Renderer.hpp>

#include <RG/Graph.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/WindowManager.hpp>

#include <Gfx/Cache/CommonRenderPass.hpp>

#include <Gfx/RenderGraph/Passes/ClearBackbufferPass.hpp>
#include <Gfx/RenderGraph/Passes/PresentBackbufferPass.hpp>

namespace Ame::Gfx
{
    void Renderer::CreateEmptyGraph()
    {
        auto& passStorage = m_Graph.get().GetPassStorage();

        passStorage.Clear();
        passStorage.NewPass<ClearBackbufferPass>("Clear Backbuffer");
        passStorage.NewPass<PresentBackbufferPass>("Present");
    }

    void Renderer::BuildRenderGraph()
    {
        constexpr Rhi::AccessLayoutStage backbufferInitialLayout{
            Rhi::AccessBits::UNKNOWN,
            Rhi::LayoutType::UNKNOWN,
            Rhi::StageBits::ALL
        };

        auto& windowManager = m_Device.get().GetWindowManager();
        auto& backbuffer    = windowManager.GetBackbuffer().get();

        auto& resourceStorage = m_Graph.get().GetResourceStorage();
        resourceStorage.ImportTexture(RG::Names::c_BackbufferImage, backbuffer.Resource, backbufferInitialLayout);

        m_EntityCompositor.get().UpdateGraph();
    }

    void Renderer::RunRenderGraph()
    {
        auto tryOutputToTexture =
            [this](
                const Ecs::Entity& entity)
        {
            auto cameraOutput = entity->get<Ecs::Component::CameraOutput>();
            if (!cameraOutput)
            {
                return;
            }

            auto& resourceStorage = m_Graph.get().GetResourceStorage();

            auto  sourceResource = resourceStorage.GetResource(RG::ResourceId(cameraOutput->SourceView));
            auto& outputTexture  = cameraOutput->OutputTexture;
            if (!sourceResource)
            {
                return;
            }

            auto sourceTexture = sourceResource->AsTexture();
            if (!sourceTexture)
            {
                return;
            }

            // output to backbuffer if needed
            if (cameraOutput->OutputToBackbuffer)
            {
                auto& windowManager = m_Device.get().GetWindowManager();
                auto& backbuffer    = windowManager.GetBackbuffer().get();

                m_CommonRenderPass.get().Blit(
                    { .SrcTexture      = sourceTexture->Resource,
                      .DstTexture      = backbuffer.Resource,
                      .SrcSubresources = { &Rhi::c_AllSubresources, 1 },
                      .DstSubresources = { &Rhi::c_AllSubresources, 1 } });
            }

            // output to texture if needed
            if (outputTexture)
            {
                m_CommonRenderPass.get().Blit(
                    { .SrcTexture      = sourceTexture->Resource,
                      .DstTexture      = *outputTexture,
                      .SrcSubresources = { &Rhi::c_AllSubresources, 1 },
                      .DstSubresources = { &Rhi::c_AllSubresources, 1 } });
            }
        };

        //

        auto renderCallback =
            [this, tryOutputToTexture](
                Ecs::Iterator                    iter,
                const Ecs::Component::Transform* transforms,
                const Ecs::Component::Camera*    cameras)
        {
            for (auto i : iter)
            {
                Ecs::Entity entity(iter.entity(i));
                m_EntityCompositor.get().RenderGraph(entity, cameras[i], transforms[i]);
                tryOutputToTexture(entity);
            }
        };

        //

        m_CameraQuery->iter(std::move(renderCallback));
    }
} // namespace Ame::Gfx