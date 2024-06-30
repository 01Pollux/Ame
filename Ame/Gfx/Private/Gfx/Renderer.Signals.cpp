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
} // namespace Ame::Gfx