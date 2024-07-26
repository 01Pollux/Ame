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

        ImplementObject<Renderable3DComponent,
                        RenderableComponent>(flecsWorld, "Ame.Renderable3DComponent");

        ImplementObject<StaticMeshComponent,
                        Renderable3DComponent>(flecsWorld, "Ame.StaticMeshComponent");
    }
} // namespace Ame::Ecs