#include <EcsModule/SceneModule.hpp>

#include <EcsComponent/Scene/RuntimeScene.hpp>
#include <EcsComponent/Scene/SceneEntity.hpp>

namespace Ame::Ecs
{
    static void OnEntitySceneChange_UpdateActiveScene(Iterator& iter)
    {
        auto world        = iter.world();
        auto currentScene = world.target<SceneEntityPairComponent>();

        while (iter.next())
        {
            for (auto i : iter)
            {
                auto entity(iter.entity(i));
                auto entityScene = entity.target<SceneEntityPairComponent>();
                if ((entityScene == currentScene) && (iter.event() == flecs::OnAdd))
                {
                    entity.add<ActiveSceneEntityTag>();
                }
                else
                {
                    entity.remove<ActiveSceneEntityTag>();
                }
            }
        }
    }

    static void OnWorldSceneEntityChanged_UpdateActiveScene(Iterator& iter)
    {
        Ecs::WorldRef world  = iter.world();
        auto          target = world->target<SceneEntityPairComponent>();
        auto          filter = world.CreateQuery().with<SceneEntityPairComponent>(target ? target : flecs::Wildcard).build();

        if (iter.event() == flecs::OnAdd)
        {
            filter.each([](auto entity) { entity.add<ActiveSceneEntityTag>(); });
        }
        else
        {
            filter.each([](auto entity) { entity.remove<ActiveSceneEntityTag>(); });
        }
    }

    //

    void SceneEcsModule::RegisterSceneObservers(WorldRef world)
    {
        world.CreateObserver()
            .with<SceneEntityPairComponent>(flecs::Wildcard)
            .singleton()
            .event(flecs::Monitor)
            .yield_existing()
            .run(OnWorldSceneEntityChanged_UpdateActiveScene);

        world.CreateObserver()
            .with<SceneEntityPairComponent>(flecs::Wildcard)
            .event(flecs::Monitor)
            .yield_existing()
            .run(OnEntitySceneChange_UpdateActiveScene);
    }
} // namespace Ame::Ecs