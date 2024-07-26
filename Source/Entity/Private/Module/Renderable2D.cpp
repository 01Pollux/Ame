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
                    auto& renderable      = *entity->get_mut<Component::BaseRenderable>();
                    renderable.CameraMask = sprite.CameraMask;
                    entity->modified<Component::BaseRenderable>();

                    auto& batchableRenderable                 = *entity->get_mut<Component::StandardBatchedRenderable>();
                    batchableRenderable.Material              = sprite.Material.get();
                    batchableRenderable.VertexBuffers.Buffer0 = sprite.Vertices;
                    batchableRenderable.IndexBuffer           = sprite.Indices;
                    entity->modified<Component::StandardBatchedRenderable>();
                });
    }
} // namespace Ame::Ecs::Module