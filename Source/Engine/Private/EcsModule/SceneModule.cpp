#include <EcsModule/SceneModule.hpp>

#include <EcsComponent/Scene/RuntimeScene.hpp>
#include <EcsComponent/Scene/SceneEntity.hpp>

namespace Ame::Ecs
{
    SceneEcsModule::SceneEcsModule(WorldRef world)
    {
        world->module<SceneEcsModule>();

        //

        world->component<RuntimeSceneComponent>().add(flecs::Relationship);
        world->component<SceneEntityPairComponent>().add(flecs::Relationship);
        world->component<ActiveSceneEntityPairComponent>().add(flecs::Relationship);
        world->component<ActiveSceneEntityTag>().add(flecs::Trait);

        //

        RegisterSceneObservers(world);
    }
} // namespace Ame::Ecs