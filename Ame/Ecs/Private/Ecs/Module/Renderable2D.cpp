#include <Ecs/Module/Renderable2D.hpp>
#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Component/Renderable/2D/Sprite.hpp>

namespace Ame::Ecs::Module
{
    Renderable2DModule::Renderable2DModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<Renderable2DModule>();

        RenderableModule::AttachRenderable(FlecsWorld.component<Component::Sprite>())
            .on_set(
                [](flecs::entity Entity, Component::Sprite& Sprite)
                {
                    Sprite.Vertex.View = Sprite.Vertices.data();
                    Sprite.Index.View  = Sprite.Indices.data();

                    Sprite.Vertex.Count = static_cast<uint32_t>(Sprite.Vertices.size());
                    Sprite.Index.Count  = static_cast<uint32_t>(Sprite.Indices.size());
                });
    }
} // namespace Ame::Ecs::Module