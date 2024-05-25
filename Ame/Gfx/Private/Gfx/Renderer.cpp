#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <Rhi/Device/Device.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        EngineFrame&   engineFrame,
        FrameTimer&    frameTimer,
        Rhi::Device&   rhiDevice,
        Ecs::Universe& universe) :
        m_Frame(engineFrame),
        m_Timer(frameTimer),
        m_Device(rhiDevice),
        m_Graph(frameTimer, rhiDevice, universe),
        m_Universe(universe)
    {
        if (!rhiDevice.IsHeadless())
        {
            m_OnWorldChange = {
                universe.OnWorldChange()
                    .ObjectSignal(),
                [this](auto& Universe, auto& ChangeData)
                {
                    m_CameraQuery.Reset();
                    if (ChangeData.NewWorld)
                    {
                        m_CameraQuery =
                            ChangeData.NewWorld
                                ->CreateQuery<const Ecs::Component::Transform, const Ecs::Component::Camera>()
                                .order_by<const Ecs::Component::Camera>(
                                    [](flecs::entity_t, auto a, flecs::entity_t, auto b) -> int
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

    RG::Graph& Renderer::GetRenderGraph()
    {
        return m_Graph;
    }

    const RG::Graph& Renderer::GetRenderGraph() const
    {
        return m_Graph;
    }

    //

    void Renderer::OnUpdate()
    {
        m_Graph.Update();
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
        auto renderIter =
            [this](Ecs::Iterator                    iter,
                   const Ecs::Component::Transform* transforms,
                   const Ecs::Component::Camera*    cameras)
        {
            for (auto i : iter)
            {
                m_Graph.UpdateFrameStorage(
                    iter.entity(i),
                    transforms[i],
                    cameras[i].GetProjectionMatrix(),
                    cameras[i].GetViewporSize());
                m_Graph.Execute();
            }
        };
        m_CameraQuery->iter(std::move(renderIter));
    }

    void Renderer::OnEndFrame()
    {
        m_Device.get().EndFrame();
    }
} // namespace Ame::Gfx