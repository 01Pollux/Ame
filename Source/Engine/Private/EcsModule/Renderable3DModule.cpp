#include <EcsModule/Renderable3DModule.hpp>

#include <EcsComponent/Core/BaseObject.hpp>

#include <EcsComponent/Renderables/BaseRenderable3D.hpp>
#include <EcsComponent/Renderables/3D/StaticMesh.hpp>

namespace Ame::Ecs
{
    Renderable3DEcsModule::Renderable3DEcsModule(flecs::world& flecsWorld)
    {
        flecsWorld.module<Renderable3DEcsModule>();

        ImplementObject<IBaseRenderable3DComponent, IBaseRenderableComponent>(flecsWorld);

        ImplementObject<StaticMeshComponent, IBaseRenderable3DComponent>(flecsWorld);
    }
} // namespace Ame::Ecs