#include <EcsModule/Renderable3DModule.hpp>

#include <EcsComponent/Core/BaseObject.hpp>

#include <EcsComponent/Renderables/BaseRenderable3D.hpp>
#include <EcsComponent/Renderables/3D/StaticMesh.hpp>

namespace Ame::Ecs
{
    Renderable3DEcsModule::Renderable3DEcsModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<Renderable3DEcsModule>();

        // ImplementObject<Renderable3DComponent,
        //                 RenderableComponent>(flecsWorld, "Ame.Renderable3DComponent");

        // ImplementObject<StaticMeshComponent,
        //                 Renderable3DComponent>(flecsWorld, "Ame.StaticMeshComponent");

        flecsWorld.component<Renderable3DComponent>()
            .on_set([](flecs::entity e, Renderable3DComponent& c)
                    { e.emplace<RenderableComponent>(c.Object); })
            .on_remove([](flecs::entity e, Renderable3DComponent&)
                       { e.remove<RenderableComponent>(); })
            .is_a<RenderableComponent>();

        flecsWorld.component<StaticMeshComponent>()
            .on_set([](flecs::entity e, StaticMeshComponent& c)
                    { e.emplace<Renderable3DComponent>(c.Object.RawPtr()); })
            .on_remove([](flecs::entity e, StaticMeshComponent&)
                       { e.remove<Renderable3DComponent>(); })
            .is_a<Renderable3DComponent>();
    }
} // namespace Ame::Ecs