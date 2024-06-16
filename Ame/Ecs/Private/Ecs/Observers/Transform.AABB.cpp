#include <Ecs/Module/Math.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Math/AABB.hpp>

#include <Ecs/World.hpp>

namespace Ame::Ecs::Module
{
    void MathModule::RegisterTransformObserver(
        flecs::world& flecsWorld)
    {
        Ecs::WorldRef world(flecsWorld.get_world());

        world.CreateObserver<const Component::Transform, const Component::AABB>()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .iter(OnTransformChanged);
    }

    //

    void MathModule::OnTransformChanged(
        Iterator&                   iter,
        const Component::Transform* transforms,
        const Component::AABB*      boxes)
    {
        for (auto i : iter)
        {
            Ecs::Entity entity(iter.entity(i));

            auto& transform = transforms[i];
            auto& box       = boxes[i];

            if (iter.event() == flecs::OnSet)
            {
                entity->emplace<Component::AABBTransformed>(box.ToAABB().Transform(transform.ToMat4x4()));
            }
            else if (iter.event() == flecs::OnRemove)
            {
                entity->remove<Component::AABBTransformed>();
            }
        }
    }
} // namespace Ame::Ecs::Module