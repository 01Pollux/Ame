#include <EcsComponent/Scene/RuntimeScene.hpp>

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
    [[nodiscard]] static String RandomGuiName()
    {
        auto uid = UIdUtils::ToString(UIdUtils::Generate());
        Strings::ReplaceAll(uid, "-", "_");
        return uid;
    }

    /// <summary>
    /// Clones an entity and all its children.
    /// </summary>
    [[nodiscard]] static Ecs::Entity CloneEntityInternal(const Ecs::Entity& sceneRoot, const Ecs::Entity& entity,
                                                         const Ecs::Entity& newParent)
    {
        auto newEntity = entity->clone();
        newEntity.add<SceneEntityPairComponent>(sceneRoot.GetFlecsEntity());
        if (newParent)
        {
            newEntity.child_of(newParent.GetFlecsEntity());
        }
        return newEntity;
    }

    /// <summary>
    /// Clones an entity and all its children while also executing a callback for each entity.
    /// </summary>
    static Ecs::Entity CloneEntityTo(const Ecs::Entity& sceneRoot, const Ecs::Entity& entity,
                                     const Ecs::Entity& newParent)
    {
        auto newEntity = CloneEntityInternal(sceneRoot, entity, newParent);
        for (auto& child : entity.GetChildren())
        {
            CloneEntityTo(sceneRoot, child, newEntity);
        }
        return newEntity;
    }

    //

    RuntimeScene::RuntimeScene(IReferenceCounters* referenceCounters, Ecs::World* world) :
        Base(referenceCounters), m_World(world), m_Root(world->CreateEntity())
    {
        m_Root.Get()->set_name(RandomGuiName().c_str());
    }

    void RuntimeScene::AddEntity(const Ecs::Entity& entity)
    {
        entity->add<SceneEntityPairComponent>(m_Root->GetFlecsEntity());
    }

    void RuntimeScene::RemoveEntity(const Ecs::Entity& entity)
    {
        if (entity->target<SceneEntityPairComponent>() == m_Root->GetFlecsEntity())
        {
            entity->remove<SceneEntityPairComponent>();
        }
    }

    Ecs::Entity RuntimeScene::CloneEntity(const Ecs::Entity& entity, bool deepClone)
    {
        if (!deepClone)
        {
            return CloneEntityInternal(m_Root, entity, m_Root);
        }
        else
        {
            return CloneEntityTo(m_Root, entity, m_Root);
        }
    }

    void RuntimeScene::MergeTo(RuntimeScene* targetScene, RuntimeSceneMerge mergeType)
    {
        switch (mergeType)
        {
        case RuntimeSceneMerge::Copy:
        {
            for (auto& entity : m_Root->GetChildren())
            {
                CloneEntityTo(targetScene->m_Root, entity, targetScene->m_Root);
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
} // namespace Ame::Ecs