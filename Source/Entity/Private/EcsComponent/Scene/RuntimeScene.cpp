#include <EcsComponent/Scene/RuntimeScene.hpp>
#include <EcsComponent/Scene/SceneEntity.hpp>
#include <Ecs/EntityUtils.hpp>
#include <Core/String.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Sander:
    /// I looked a bit more into it, turns out it's a bit of a difficult problem to address generically without
    /// complicating the APIs or hurting performance
    // One quick workaround is to give the Root entity a name, for example: world.entity("Root")
    // The reason for that is that:
    // during world cleanup root entities are cleaned up first, which should have cleaned up Root before the query
    // entity however, because the Root entity is empty, it's not stored in any archetype, which makes it really hard to
    // find in the cleanup code By adding a name (or any other component) to it, it's no longer empty and the cleanup
    // works as expected I'll add a note to the docs for now, until I come up with a better way to address this
    /// </summary>
    [[nodiscard]] static String RandomUIdName()
    {
        auto uid = UIdUtils::ToString(UIdUtils::Generate());
        Strings::ReplaceAll(uid, "-", "");
        return uid;
    }

    //

    RuntimeScene::RuntimeScene(IReferenceCounters* referenceCounters, Ecs::World* world) :
        Base(referenceCounters), m_World(world), m_Root(world->CreateEntity())
    {
        auto flecsRoot = m_Root->GetFlecsEntity();
        flecsRoot.set_name(RandomUIdName().c_str());
        flecsRoot.emplace<RuntimeSceneComponent>(Ptr{ this });
    }

    //

    Ecs::Entity RuntimeScene::CreateEntity(const StringView name)
    {
        Ecs::Entity entity;
        if (auto world = m_World.Lock())
        {
            entity = world->CreateEntity(name, m_Root);
            AddEntity(entity);
        }
        return entity;
    }

    void RuntimeScene::AddEntity(const Ecs::Entity& entity)
    {
        entity->add<SceneEntityPairComponent>(m_Root->GetFlecsEntity());
    }

    void RuntimeScene::RemoveEntity(const Ecs::Entity& entity)
    {
        if (EntityUtils::GetSceneRoot(entity) == m_Root)
        {
            entity->remove<SceneEntityPairComponent>(flecs::Wildcard);
        }
    }

    Ecs::Entity RuntimeScene::CloneEntity(const Ecs::Entity& entity, bool deepClone)
    {
        return EntityUtils::Clone(entity, m_Root, deepClone);
    }

    void RuntimeScene::MergeTo(RuntimeScene* targetScene, RuntimeSceneMerge mergeType)
    {
        switch (mergeType)
        {
        case RuntimeSceneMerge::Copy:
        {
            for (auto& entity : m_Root->GetChildren())
            {
                EntityUtils::CloneTo(targetScene->m_Root, entity, targetScene->m_Root);
            }
            break;
        }
        case RuntimeSceneMerge::Move:
        {
            for (auto& entity : m_Root->GetChildren())
            {
                entity->child_of(targetScene->m_Root->GetFlecsEntity());
            }
            break;
        }
        case RuntimeSceneMerge::Replace:
        {
            m_Root->RemoveAllChildren();
            for (auto& entity : targetScene->m_Root->GetChildren())
            {
                entity->child_of(m_Root->GetFlecsEntity());
            }
            break;
        }
        case RuntimeSceneMerge::Clear:
        {
            m_Root->RemoveAllChildren();
            break;
        }
        default:
            std::unreachable();
        }
    }

    //

    RuntimeScene* RuntimeScene::GetCurrent(Ecs::World* world)
    {
        auto scene = world->GetFlecsWorld()->target<ActiveSceneEntityPairComponent>();
        return scene ? scene.get<RuntimeSceneComponent>()->Object : nullptr;
    }

    Ecs::Entity RuntimeScene::GetCurrentEntity(Ecs::World* world)
    {
        auto scene = GetCurrent(world);
        return scene ? scene->GetRoot() : Ecs::Entity::c_Null;
    }

    void RuntimeScene::SetCurrent(Ecs::World* world, RuntimeScene* scene)
    {
        auto flecsWorld = world->GetFlecsWorld();
        if (scene)
        {
            flecsWorld->add<ActiveSceneEntityPairComponent>(scene->GetRoot().GetFlecsEntity());
        }
        else
        {
            flecsWorld->remove<ActiveSceneEntityPairComponent>(flecs::Wildcard);
        }
    }

    void RuntimeScene::SetCurrent()
    {
        if (auto world = m_World.Lock())
        {
            SetCurrent(world, this);
        }
    }
} // namespace Ame::Ecs