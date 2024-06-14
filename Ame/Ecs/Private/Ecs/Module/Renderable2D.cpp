#include <Ecs/Module/Renderable2D.hpp>
#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Component/Renderable/2D/Sprite.hpp>

#include <Ecs/Entity.hpp>

namespace Ame::Ecs::Module
{
    Renderable2DModule::Renderable2DModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<Renderable2DModule>();

        flecsWorld.component<Component::Sprite>()
            .on_set(
                [](Ecs::Entity entity, Component::Sprite& sprite)
                {
                    auto& renderable      = entity.GetComponentMut<Component::BaseRenderable>();
                    renderable.CameraMask = sprite.CameraMask;
                    entity.MarkModified<Component::BaseRenderable>();
                });
    }
} // namespace Ame::Ecs::Module