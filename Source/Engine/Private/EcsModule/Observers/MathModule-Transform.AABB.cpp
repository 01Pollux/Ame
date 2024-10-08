#include <EcsModule/MathModule.hpp>

#include <EcsComponent/Math/Transform.hpp>
#include <EcsComponent/Math/AABB.hpp>
#include <EcsComponent/Scene/SceneEntity.hpp>

namespace Ame::Ecs
{
    static void OnTransformChanged_UpdateGlobal(Iterator& iter)
    {
        while (iter.next())
        {
            auto localTransforms  = iter.field<const TransformComponent>(0);
            auto parentTransforms = iter.field<const GlobalTransformComponent>(1);
            for (auto i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto& localTransform  = localTransforms[i];
                auto  parentTransform = iter.is_set(1) ? &parentTransforms[i] : nullptr;

                if (iter.event() == flecs::OnSet)
                {
                    if (parentTransform)
                    {
                        entity->set(
                            GlobalTransformComponent{ parentTransform->ToMat4x4() * localTransform.ToMat4x4() });
                    }
                    else
                    {
                        entity->set(GlobalTransformComponent{ localTransform.ToMat4x4() });
                    }
                }
                else if (iter.event() == flecs::OnRemove)
                {
                    entity->remove<GlobalTransformComponent>();
                }
            }
        }
    }

    static void OnTransformChanged_ChangeAABB(Iterator& iter)
    {
        while (iter.next())
        {
            auto transforms = iter.field<const GlobalTransformComponent>(0);
            auto boxes      = iter.field<const AABBComponent>(1);
            for (auto i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto& transform = transforms[i];
                auto& box       = boxes[i];

                if (iter.event() == flecs::OnSet)
                {
                    entity->set<TransformedAABBComponent>({ box.ToAABB().Transform(transform.ToMat4x4()) });
                }
                else if (iter.event() == flecs::OnRemove)
                {
                    entity->remove<TransformedAABBComponent>();
                }
            }
        }
    }

    //

    void MathEcsModule::RegisterTransformObservers(WorldRef world)
    {
        // Register global transform observer, to update global transform when transform changes
        world.CreateObserver<const TransformComponent, const GlobalTransformComponent*>()
            .term_at(1) ///
            .parent()   // GlobalTransformComponent is a parent of TransformComponent
            .cascade()  ///
            .with<ActiveSceneEntityTag>()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .run(OnTransformChanged_UpdateGlobal);

        // Register transformed AABB observer, to update AABB when transform changes
        world.CreateObserver<const GlobalTransformComponent, const AABBComponent>()
            .with<ActiveSceneEntityTag>()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .run(OnTransformChanged_ChangeAABB);
    }
} // namespace Ame::Ecs