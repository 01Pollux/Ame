#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <Rhi/Device/Device.hpp>
#include <Rhi/Staging/DeferredStagingManager.hpp>
#include <Gfx/RG/Graph.hpp>
#include <Gfx/Cache/CommonRenderPass.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        EngineFrame&                          engineFrame,
        FrameTimer&                           frameTimer,
        Rhi::Device&                          rhiDevice,
        Rhi::Staging::DeferredStagingManager& stagingManager,
        Ecs::Universe&                        universe,
        RG::Graph&                            renderGraph,
        Cache::CommonRenderPass&              commonRenderPass) :
        m_Frame(engineFrame),
        m_Timer(frameTimer),
        m_Device(rhiDevice),
        m_Universe(universe),
        m_StagingManager(stagingManager),
        m_Graph(renderGraph),
        m_CommonRenderPass(commonRenderPass)
    {
        if (!rhiDevice.IsHeadless())
        {
            m_OnWorldChange = {
                universe.OnWorldChange()
                    .ObjectSignal(),
                [this](auto& universe, auto& changeData)
                {
                    m_CameraQuery.Reset();
                    if (changeData.NewWorld)
                    {
                        m_CameraQuery =
                            changeData.NewWorld
                                ->CreateQuery<const Ecs::Component::Transform, const Ecs::Component::Camera>()
                                .order_by<const Ecs::Component::Camera>(
                                    [](flecs::entity_t, auto a,
                                       flecs::entity_t, auto b) -> int
                                    {
                                        return a->Priority - b->Priority;
                                    })
                                .build();
                    }
                }
            };

            m_OnUpdate = {
                engineFrame.OnUpdate()
                    .ObjectSignal(),
                [this]
                { OnUpdate(); }
            };

            m_OnStartFrame = {
                engineFrame.OnStartFrame()
                    .ObjectSignal(),
                [this]
                { OnStartFrame(); }
            };

            m_OnRender = {
                engineFrame.OnRender()
                    .ObjectSignal(),
                [this]
                { OnRender(); }
            };

            m_OnEndFrame = {
                engineFrame.OnEndFrame()
                    .ObjectSignal(),
                [this]
                { OnEndFrame(); }
            };
        }
    }

    //

    void Renderer::OnUpdate()
    {
        m_Graph.get().Update();
    }

    void Renderer::OnStartFrame()
    {
        if (!m_Device.get().ProcessEvents()) [[unlikely]]
        {
            m_Frame.get().Stop();
        }
        else
        {
            m_Device.get().BeginFrame();
        }
    }

    void Renderer::OnRender()
    {
        FlushDeferredUploads();
        RunRenderGraph();
    }

    void Renderer::OnEndFrame()
    {
        m_Device.get().EndFrame();
    }

    //

    void Renderer::FlushDeferredUploads()
    {
        m_StagingManager.get().Flush();
    }

    void Renderer::RunRenderGraph()
    {
        Ecs::Entity lastCamera;

        //

        auto tryOutputToTexture =
            [&](const Ecs::Entity& entity)
        {
            auto cameraOutput = entity.TryGetComponent<Ecs::Component::CameraOutput>();
            if (!cameraOutput)
            {
                return;
            }

            auto& resourceStorage = m_Graph.get().GetResourceStorage();

            auto  sourceResource = resourceStorage.GetResource(RG::ResourceId(cameraOutput->SourceView));
            auto& outputTexture  = cameraOutput->OutputTexture;
            if (!sourceResource || !outputTexture)
            {
                return;
            }

            auto sourceTexture = sourceResource->AsTexture();
            if (!sourceTexture)
            {
                return;
            }

            m_CommonRenderPass.get().Blit(
                { .SrcTexture      = *sourceTexture,
                  .DstTexture      = *outputTexture,
                  .SrcSubresources = { &Rhi::c_AllSubresources, 1 },
                  .DstSubresources = { &Rhi::c_AllSubresources, 1 } });
        };

        auto tryOutputToBackbuffer =
            [&](const Ecs::Entity& entity)
        {
            auto cameraOutput = entity.TryGetComponent<Ecs::Component::CameraOutput>();
            if (!cameraOutput)
            {
                return;
            }

            auto& resourceStorage = m_Graph.get().GetResourceStorage();

            auto  sourceResource = resourceStorage.GetResource(RG::ResourceId(cameraOutput->SourceView));
            auto& outputTexture  = m_Device.get().GetBackbuffer().Resource;
            if (!sourceResource || !outputTexture)
            {
                return;
            }

            auto sourceTexture = sourceResource->AsTexture();
            if (!sourceTexture)
            {
                return;
            }

            m_CommonRenderPass.get().Blit(
                { .SrcTexture      = *sourceTexture,
                  .DstTexture      = outputTexture,
                  .SrcSubresources = { &Rhi::c_AllSubresources, 1 },
                  .DstSubresources = { &Rhi::c_AllSubresources, 1 } });
        };

        //

        auto renderIter =
            [&](Ecs::Iterator                    iter,
                const Ecs::Component::Transform* transforms,
                const Ecs::Component::Camera*    cameras)
        {
            for (auto i : iter)
            {
                Ecs::Entity entity(iter.entity(i));
                lastCamera = entity;

                auto& curTransform = transforms[i];
                auto& curCamera    = cameras[i];

                m_Graph.get().UpdateFrameStorage(
                    entity,
                    curTransform,
                    curCamera.GetProjectionMatrix(),
                    curCamera.GetViewporSize());
                m_Graph.get().Execute();

                tryOutputToTexture(entity);
            }
        };

        m_CameraQuery->iter(std::move(renderIter));

        if (lastCamera)
        {
            tryOutputToBackbuffer(lastCamera);
        }
    }
} // namespace Ame::Gfx