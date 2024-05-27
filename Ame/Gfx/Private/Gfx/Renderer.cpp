#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <Rhi/Device/Device.hpp>
#include <Gfx/RG/Graph.hpp>
#include <Gfx/Cache/CommonRenderPass.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        EngineFrame&             engineFrame,
        FrameTimer&              frameTimer,
        Rhi::Device&             rhiDevice,
        Ecs::Universe&           universe,
        RG::Graph&               renderGraph,
        Cache::CommonRenderPass& commonRenderPass) :
        m_Frame(engineFrame),
        m_Timer(frameTimer),
        m_Device(rhiDevice),
        m_Universe(universe),
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
        RunRenderGraph();
    }

    void Renderer::OnEndFrame()
    {
        m_Device.get().EndFrame();
    }

    //

    void Renderer::RunRenderGraph()
    {
        const Ecs::Component::Camera* lastCamera = nullptr;

        auto outputToTexture =
            [&](const Ecs::Component::CameraOutput& cameraOutput)
        {
            auto& resourceStorage = m_Graph.get().GetResourceStorage();

            auto& outputTexture = cameraOutput.OutputTexture;
            auto  sourceTexture = resourceStorage.GetResource(RG::ResourceId(cameraOutput.SourceView)).AsTexture();

            if (!outputTexture || sourceTexture)
            {
                return;
            }

            m_CommonRenderPass.get().Blit(
                Cache::SingleBlitParameters{
                    .SrcTexture = *sourceTexture,
                    .DstTexture = *outputTexture });
        };

        auto renderIter =
            [&](Ecs::Iterator                    iter,
                const Ecs::Component::Transform* transforms,
                const Ecs::Component::Camera*    cameras)
        {
            for (auto i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto& curTransform = transforms[i];
                auto& curCamera    = cameras[i];
                auto  cameraOutput = entity.TryGetComponent<Ecs::Component::CameraOutput>();
                lastCamera         = &curCamera;

                m_Graph.get().UpdateFrameStorage(
                    entity,
                    curTransform,
                    curCamera.GetProjectionMatrix(),
                    curCamera.GetViewporSize());
                m_Graph.get().Execute();

                if (cameraOutput)
                {
                    outputToTexture(*cameraOutput);
                }
            }
        };

        m_CameraQuery->iter(std::move(renderIter));

        if (lastCamera)
        {
        }
    }
} // namespace Ame::Gfx