#include <Gfx/RG/Ecs/System.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>

namespace Ame::Gfx::RG
{
    void EcsSystemHooks::CreateCameraRule()
    {
        auto& World = *m_Universe.get().GetActiveWorld();

        m_WorldData.RenderRule =
            World.CreateRule<
                     RenderInstance,
                     const Ecs::Component::Transform,
                     const Ecs::Component::BaseRenderable>()
                .build();
    }
} // namespace Ame::Gfx::RG