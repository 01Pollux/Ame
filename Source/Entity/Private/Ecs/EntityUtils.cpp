#include <Ecs/EntityUtils.hpp>
#include <EcsComponent/Scene/SceneEntity.hpp>

namespace Ame::Ecs
{
    [[nodiscard]] static Ecs::Entity CloneEntityInternal(const Ecs::Entity& sceneRoot, const Ecs::Entity& entity,
                                                         const Ecs::Entity& newParent)
    {
        auto newEntity = entity->clone();
        if (sceneRoot)
        {
            newEntity.add<SceneEntityPairComponent>(sceneRoot.GetFlecsEntity());
        }
        if (newParent)
        {
            newEntity.child_of(newParent.GetFlecsEntity());
        }
        return newEntity;
    }

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

    Entity EntityUtils::GetSceneRoot(const Entity& entity)
    {
        return entity->target<SceneEntityPairComponent>();
    }

    Entity EntityUtils::Clone(const Entity& entity, Entity newParent, bool deepClone)
    {
        return CloneTo(GetSceneRoot(entity), entity, newParent, deepClone);
    }

    Entity EntityUtils::CloneTo(const Entity& sceneRoot, const Entity& entity, Entity newParent, bool deepClone)
    {
        if (!newParent)
        {
            newParent = sceneRoot;
        }
        return deepClone ? CloneEntityTo(sceneRoot, entity, newParent)
                         : CloneEntityInternal(sceneRoot, entity, newParent);
    }
} // namespace Ame::Ecs