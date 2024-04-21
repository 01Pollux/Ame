#include <Ecs/Module/Geometry2D.hpp>

#include <Ecs/Component/Geometry2D/Sprite.hpp>

namespace Ame::Ecs::Module
{
    Geometry2DModule::Geometry2DModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<Geometry2DModule>();

        FlecsWorld.component<Component::Sprite>();
    }
} // namespace Ame::Ecs::Module