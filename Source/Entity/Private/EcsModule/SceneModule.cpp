#include <EcsModule/SceneModule.hpp>

#include <EcsComponent/Scene/RuntimeScene.hpp>
#include <EcsComponent/Scene/SceneEntity.hpp>

namespace Ame::Ecs
{
    SceneEcsModule::SceneEcsModule(WorldRef world)
    {
        world->module<SceneEcsModule>();

        //

        world->component<RuntimeSceneComponent>("Ame.RuntimeSceneComponent").add(flecs::Relationship);
        world->component<SceneEntityPairComponent>("Ame.SceneEntityPairComponent").add(flecs::Relationship);
        world->component<ActiveSceneEntityPairComponent>("Ame.ActiveSceneEntityPairComponent").add(flecs::Relationship);
        world->component<ActiveSceneEntityTag>("Ame.ActiveSceneEntityTag").add(flecs::Trait);

        //

        RegisterSceneObservers(world);
    }
} // namespace Ame::Ecs