#include <EcsModule/MathModule.hpp>

#include <EcsComponent/Math/TransformComponent.hpp>
#include <EcsComponent/Math/AABBComponent.hpp>

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    static void OnTransformChanged_UpdateGlobal(
        Iterator& iter)
    {
        while (iter.next())
        {
            auto localTransforms  = iter.field<TransformComponent>(0);
            auto parentTransforms = iter.field<GlobalTransformComponent>(1);
            for (auto i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                auto& localTransform  = localTransforms[i];
                auto& parentTransform = parentTransforms[i];

                if (iter.event() == flecs::OnSet)
                {
                    entity->set<GlobalTransformComponent>({ parentTransform.ToMat4x4() * localTransform.ToMat4x4() });
                }
                else if (iter.event() == flecs::OnRemove)
                {
                    entity->remove<GlobalTransformComponent>();
                }
            }
        }
    }

    static void OnTransformChanged_ChangeAABB(
        Iterator& iter)
    {
        while (iter.next())
        {
            auto transforms = iter.field<GlobalTransformComponent>(0);
            auto boxes      = iter.field<AABBComponent>(1);
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

    void MathEcsModule::RegisterTransformObservers(
        flecs::world& flecsWorld)
    {
        Ecs::WorldRef world(flecsWorld.get_world());

        // Register global transform observer, to update global transform when transform changes
        world.CreateObserver<const TransformComponent, const GlobalTransformComponent*>()
            .term_at(1) ///
            .parent()   // GlobalTransformComponent is a parent of TransformComponent
            .cascade()  ///
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .run(OnTransformChanged_UpdateGlobal);

        // Register transformed AABB observer, to update AABB when transform changes
        world.CreateObserver<const GlobalTransformComponent, const AABBComponent>()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .run(OnTransformChanged_ChangeAABB);
    }
} // namespace Ame::Ecs