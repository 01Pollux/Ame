#include <Ecs/Module/Renderable2D.hpp>
#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Component/Renderable/2D/Sprite.hpp>

#include <Ecs/Entity.hpp>

namespace Ame::Ecs::Module
{
    Renderable2DModule::Renderable2DModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<Renderable2DModule>();

        FlecsWorld.component<Component::Sprite>()
            .on_set(
                [](Ecs::Entity Entity, Component::Sprite& Sprite)
                {
                    auto& Renderable = Entity.GetComponentMut<Component::BaseRenderable>();

                    Renderable.Vertex = Component::BaseRenderable::BufferView::Local(Sprite.Vertices.data(), Sprite.Vertices.size(), sizeof(Sprite.Vertices[0]));
                    Renderable.Index  = Component::BaseRenderable::BufferView::Local(Sprite.Indices.data(), Sprite.Indices.size(), sizeof(Sprite.Indices[0]));

                    Renderable.Material   = Sprite.Material;
                    Renderable.CameraMask = Sprite.CameraMask;

                    Entity.MarkModified<Component::BaseRenderable>();
                });
    }
} // namespace Ame::Ecs::Module