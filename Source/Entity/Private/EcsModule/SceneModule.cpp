#include <EcsModule/SceneModule.hpp>

#include <EcsComponent/Scene/RuntimeScene.hpp>

namespace Ame::Ecs
{
    SceneEcsModule::SceneEcsModule(WorldRef world)
    {
        world->module<SceneEcsModule>();

        //

        world->component<RuntimeSceneComponent>("Ame.RuntimeSceneComponent");
        world->component<SceneEntityPairComponent>("Ame.SceneEntityPairComponent");
        world->component<ActiveEntityTag>("Ame.SceneEntityPairComponent");

        //

        RegisterSceneObservers(world);
    }
} // namespace Ame::Ecs