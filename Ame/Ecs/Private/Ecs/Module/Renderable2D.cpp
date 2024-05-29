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
                    auto& renderable = entity.GetComponentMut<Component::BaseRenderable>();

                    renderable.Vertex = Component::BaseRenderable::BufferView::Local(
                        std::bit_cast<const std::byte*>(sprite.Vertices.data()),
                        sprite.Vertices.size(),
                        sizeof(sprite.Vertices[0]));

                    renderable.Index = Component::BaseRenderable::BufferView::Local(
                        std::bit_cast<const std::byte*>(sprite.Indices.data()),
                        sprite.Indices.size(),
                        sizeof(sprite.Indices[0]));

                    renderable.Material   = sprite.Material;
                    renderable.CameraMask = sprite.CameraMask;

                    entity.MarkModified<Component::BaseRenderable>();
                });
    }
} // namespace Ame::Ecs::Module