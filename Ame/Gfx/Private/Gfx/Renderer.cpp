#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <Rhi/Device/Device.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        EngineFrame&   Frame,
        FrameTimer&    Timer,
        Rhi::Device&   Device,
        Ecs::Universe& Universe) :
        m_Frame(Frame),
        m_Timer(Timer),
        m_Device(Device),
        m_Graph(Timer, Device, Universe),
        m_Universe(Universe)
    {
        if (!Device.IsHeadless())
        {
            m_OnWorldChange = {
                Universe.OnWorldChange()
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
                Frame.OnUpdate()
                    .ObjectSignal(),
                [this]
                { OnUpdate(); }
            };

            m_OnStartFrame = {
                Frame.OnStartFrame()
                    .ObjectSignal(),
                [this]
                { OnStartFrame(); }
            };

            m_OnRender = {
                Frame.OnRender()
                    .ObjectSignal(),
                [this]
                { OnRender(); }
            };

            m_OnEndFrame = {
                Frame.OnEndFrame()
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
        auto RenderIter =
            [this](Ecs::Iterator                    Iter,
                   const Ecs::Component::Transform* Transforms,
                   const Ecs::Component::Camera*    Cameras)
        {
            for (auto i : Iter)
            {
                m_Graph.UpdateFrameStorage(
                    Iter.entity(i),
                    Transforms[i],
                    Cameras[i].GetProjectionMatrix(),
                    Cameras[i].GetViewporSize());
                m_Graph.Execute();
            }
        };
        m_CameraQuery->iter(std::move(RenderIter));
    }

    void Renderer::OnEndFrame()
    {
        m_Device.get().EndFrame();
    }
} // namespace Ame::Gfx