#include <Gfx/Renderer.hpp>

#include <Frame/EngineFrame.hpp>
#include <RG/Graph.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

namespace Ame::Gfx
{
    void Renderer::OnWorldChange(
        const Signals::Data::WorldChangeData& changeData)
    {
        m_CameraQuery.Reset();
        if (changeData.NewWorld)
        {
            // Sort cameras by priority
            m_CameraQuery =
                changeData.NewWorld
                    ->CreateQuery<const Ecs::Component::Transform,
                                  const Ecs::Component::Camera>()
                    .order_by<const Ecs::Component::Camera>(
                        [](Ecs::Entity::Id, auto a,
                           Ecs::Entity::Id, auto b) -> int
                        {
                            return a->Priority - b->Priority;
                        })
                    .build();
        }
    }

    void Renderer::OnUpdate()
    {
        m_EntityCompositor.get().UpdateGraph();
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
} // namespace Ame::Gfx