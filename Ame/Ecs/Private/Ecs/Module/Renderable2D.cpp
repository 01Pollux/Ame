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

        RenderableModule::AttachRenderable(FlecsWorld.component<Component::Sprite>())
            .on_set(
                [](Ecs::Entity Entity, Component::Sprite& Sprite)
                {
                    auto& Renderable = Entity.GetComponentMut<Component::BaseRenderable>();

                    Renderable.Vertex.View  = Sprite.Vertices.data();
                    Renderable.Vertex.Count = static_cast<uint32_t>(Sprite.Vertices.size());

                    Renderable.Index.View  = Sprite.Indices.data();
                    Renderable.Index.Count = static_cast<uint32_t>(Sprite.Indices.size());

                    Renderable.PipelineState = Sprite.PipelineState;
                    Renderable.CameraMask    = Sprite.CameraMask;
                });
    }
} // namespace Ame::Ecs::Module