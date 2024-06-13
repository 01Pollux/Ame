#include <Gfx/Ecs/System.hpp>

namespace Ame::Gfx
{
    void EcsSystemHooks::CreateCameraRule(
        Ecs::World& world)
    {
        m_WorldData.RenderRule =
            world.CreateRule<
                     const Ecs::Component::Transform,
                     const TransformBuffer::GpuId,
                     const Ecs::Component::BaseRenderable>()
                .build();
    }
} // namespace Ame::Gfx