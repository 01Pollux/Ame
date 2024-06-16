#include <Gfx/Renderer.hpp>

#include <RG/Graph.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

#include <Gfx/Cache/CommonRenderPass.hpp>

namespace Ame::Gfx
{
    void Renderer::RunRenderGraph()
    {
        auto tryOutputToTexture =
            [&](const Ecs::Entity& entity)
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
                auto& outputTexture = m_Device.get().GetBackbuffer().Resource;
                if (outputTexture)
                {
                    m_CommonRenderPass.get().Blit(
                        { .SrcTexture      = *sourceTexture,
                          .DstTexture      = outputTexture,
                          .SrcSubresources = { &Rhi::c_AllSubresources, 1 },
                          .DstSubresources = { &Rhi::c_AllSubresources, 1 } });
                }
            }

            // output to texture if needed
            if (outputTexture)
            {
                m_CommonRenderPass.get().Blit(
                    { .SrcTexture      = *sourceTexture,
                      .DstTexture      = *outputTexture,
                      .SrcSubresources = { &Rhi::c_AllSubresources, 1 },
                      .DstSubresources = { &Rhi::c_AllSubresources, 1 } });
            }
        };

        //

        auto renderCallback =
            [&](Ecs::Iterator                    iter,
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

        m_CameraQuery->iter(std::move(renderCallback));
    }
} // namespace Ame::Gfx