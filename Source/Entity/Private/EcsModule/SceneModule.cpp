#include <EcsModule/SceneModule.hpp>

#include <EcsComponent/Scene/RuntimeScene.hpp>
#include <EcsComponent/Scene/CurrentRuntimeScene.hpp>

namespace Ame::Ecs
{
    SceneEcsModule::SceneEcsModule(WorldRef world)
    {
        world->module<SceneEcsModule>();

        //

        world->component<RuntimeSceneComponent>("Ame.RuntimeSceneComponent");
        world->component<SceneEntityPairComponent>("Ame.SceneEntityPairComponent");
        world->component<CurrentRuntimeScenePairComponent>("Ame.CurrentRuntimeScenePairComponent");
    }
} // namespace Ame::Ecs