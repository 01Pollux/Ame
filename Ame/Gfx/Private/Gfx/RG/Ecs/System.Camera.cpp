#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

namespace Ame::Gfx::RG
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
} // namespace Ame::Gfx::RG