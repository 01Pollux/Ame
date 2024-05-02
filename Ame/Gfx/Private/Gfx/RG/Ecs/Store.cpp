#include <Gfx/RG/Ecs/Store.hpp>

namespace Ame::Gfx::RG
{
    EcsStore::EcsStore(
        Ecs::World& World)
    {
        m_RenderableRule =
            World.CreateRule()
                .build();
    }
} // namespace Ame::Gfx::RG