#include <Ecs/Module/Viewport.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

#include <Ecs/World.hpp>

namespace Ame::Ecs::Module
{
    void ViewportModule::RegisterTransformObserver(
        flecs::world& flecsWorld)
    {
        Ecs::WorldRef world(flecsWorld.get_world());

        world.CreateObserver<const Component::Transform>()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .with<Component::Camera>()
            .yield_existing()
            .iter(OnTransformChanged);
    }

    //

    void ViewportModule::OnTransformChanged(
        Iterator&                   iter,
        const Component::Transform* transforms)
    {
        for (auto i : iter)
        {
            Ecs::Entity entity(iter.entity(i));

            if (iter.event() == flecs::OnSet)
            {
                auto& camera    = *entity->get<Component::Camera>();
                auto& transform = transforms[i];

                entity->emplace<Component::CameraFrustum>(Geometry::Frustum(camera.GetProjectionMatrix()).Transform(transform.ToMat4x4()));
            }
            else if (iter.event() == flecs::OnRemove)
            {
                entity->remove<Component::CameraFrustum>();
            }
        }
    }
} // namespace Ame::Ecs::Module