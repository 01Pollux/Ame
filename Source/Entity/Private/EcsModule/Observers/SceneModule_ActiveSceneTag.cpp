#include <EcsModule/SceneModule.hpp>

#include <EcsComponent/Scene/RuntimeScene.hpp>

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
                if ((entityScene == currentScene) && (iter.event() == flecs::OnSet))
                {
                    entity.add<ActiveEntityTag>();
                }
                else
                {
                    entity.remove<ActiveEntityTag>();
                }
            }
        }
    }

    static void OnWorldSceneEntityChanged_UpdateActiveScene(Iterator& iter)
    {
        auto world  = iter.world();
        auto target = world.target<SceneEntityPairComponent>();
        auto filter = world.query_builder<SceneEntityPairComponent>(target).build();

        if (iter.event() == flecs::OnSet)
        {
            filter.each([](auto& entity) { entity.add<ActiveEntityTag>(); });
        }
        else
        {
            filter.each([](auto& entity) { entity.remove<ActiveEntityTag>(); });
        }
    }

    //

    void SceneEcsModule::RegisterSceneObservers(WorldRef world)
    {
        world->observer()
            .with<SceneEntityPairComponent>(flecs::Wildcard)
            .singleton()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .run(OnWorldSceneEntityChanged_UpdateActiveScene);

        world->observer()
            .with<SceneEntityPairComponent>(flecs::Wildcard)
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .run(OnEntitySceneChange_UpdateActiveScene);
    }
} // namespace Ame::Ecs