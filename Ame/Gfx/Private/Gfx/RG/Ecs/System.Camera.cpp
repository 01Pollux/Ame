#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

namespace Ame::Gfx::RG
{
    void EcsSystemHooks::CreateCameraRule()
    {
        auto& world = *m_Universe.get().GetActiveWorld();

        m_WorldData.RenderRule =
            world.CreateRule<
                     RenderInstance,
                     const Ecs::Component::Transform,
                     const Ecs::Component::BaseRenderable>()
                .build();
    }
} // namespace Ame::Gfx::RG