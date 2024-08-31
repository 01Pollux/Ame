#include <Interfaces/Graphics/RendererImpl.hpp>

#include <CommonStates/CommonRenderPasses.hpp>
#include <Rg/Graph.hpp>

#include <EcsComponent/Math/TransformComponent.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>
#include <EcsComponent/Viewport/Camera.hpp>

#include <Rhi/ImGui/ImGuiFrameContext.hpp>
#include <imgui.h>

namespace Ame::Interfaces
{
    static void TryOutputToTexture(Rhi::BlitRenderPass* blitPass, const Rg::Graph& renderGraph,
                                   const Ecs::CameraOutputComponent& cameraOutput)
    {
        auto& resourceStorage = renderGraph.GetResourceStorage();

        auto sourceResource = resourceStorage.GetResource(Rg::ResourceId(cameraOutput.SourceView));

        auto  sourceTexture = sourceResource ? sourceResource->AsTexture() : nullptr;
        auto& outputTexture = cameraOutput.OutputTexture;

        // output to texture if needed
        if (sourceTexture && outputTexture)
        {
            Rhi::BlitCopyParameters parameters{ sourceTexture, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                                outputTexture, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION };
            blitPass->Blit(parameters);
        }
    }

    static void TryOutputToBackbuffer(Dg::ISwapChain* swapChain, Rhi::BlitRenderPass* blitPass,
                                      const Rg::Graph& renderGraph, const Ecs::CameraOutputComponent& cameraOutput)
    {
        auto& resourceStorage = renderGraph.GetResourceStorage();

        auto sourceResource = resourceStorage.GetResource(Rg::ResourceId(cameraOutput.SourceView));

        auto sourceTexture = sourceResource ? sourceResource->AsTexture() : nullptr;

        if (!sourceTexture)
        {
            return;
        }

        auto srv = sourceTexture->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE);
        if (srv)
        {
            Rhi::BlitDrawParameters parameters{ sourceTexture->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE),
                                                Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                                swapChain->GetCurrentBackBufferRTV(),
                                                Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION };
            blitPass->Blit(parameters);
        }
    }

    //

    void RendererImpl::CreateCameraQuery()
    {
        m_CameraQuery =
            m_World
                ->CreateQuery<const Ecs::GlobalTransformComponent, const Ecs::CameraComponent,
                              const Ecs::CameraOutputComponent>()
                .order_by<const Ecs::CameraComponent>([](Ecs::EntityId, auto a, Ecs::EntityId, auto b) -> int
                                                      { return a->Priority - b->Priority; })
                .build();
    }

    void RendererImpl::RunRenderGraph()
    {
        const Ecs::CameraOutputComponent* lastOutput = nullptr;
        const Rg::Graph*                  lastGraph  = nullptr;

        auto renderCallback = [&](Ecs::Iterator& iter)
        {
            while (iter.next())
            {
                auto transforms = iter.field<const Ecs::GlobalTransformComponent>(0);
                auto cameras    = iter.field<const Ecs::CameraComponent>(1);
                auto outputs    = iter.field<const Ecs::CameraOutputComponent>(2);

                for (auto i : iter)
                {
                    if (cameras[i].RenderGraph)
                    {
                        m_EntityCompositor.RenderGraph(*cameras[i].RenderGraph, cameras[i], transforms[i]);
                        TryOutputToTexture(m_CommonRenderPass, *cameras[i].RenderGraph, outputs[i]);

                        lastGraph  = cameras[i].RenderGraph;
                        lastOutput = &outputs[i];
                    }
                }
            }
        };

        //

        m_EntityCompositor.Update();
        m_CameraQuery->run(renderCallback);

#ifndef AME_NO_IMGUI
        if (m_ImGuiRenderer->RenderBackbufferToTexture())
        {
            return;
        }
        else
#endif
            if (lastOutput)
        {
            TryOutputToBackbuffer(m_RhiDevice->GetSwapchain(), m_CommonRenderPass, *lastGraph, *lastOutput);
        }
    }
} // namespace Ame::Interfaces