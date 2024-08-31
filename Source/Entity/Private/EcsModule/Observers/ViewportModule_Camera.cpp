#include <EcsModule/ViewportModule.hpp>

#include <EcsComponent/Math/TransformComponent.hpp>
#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Viewport/CameraFrustum.hpp>

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    static void OnCameraTransformChanged_UpdateFrustum(Iterator& iter)
    {
        while (iter.next())
        {
            auto transforms = iter.field<const GlobalTransformComponent>(0);
            auto cameras    = iter.field<const CameraComponent>(1);

            for (auto i : iter)
            {
                Ecs::Entity entity(iter.entity(i));

                if (iter.event() == flecs::OnSet)
                {
                    Geometry::Frustum frustum(cameras[i].GetProjectionMatrix());
                    entity->set(CameraFrustumComponent{ frustum.Transform(transforms[i].ToMat4x4()) });
                }
                else if (iter.event() == flecs::OnRemove)
                {
                    entity->remove<CameraFrustumComponent>();
                }
            }
        }
    }

    //

    void ViewporEcsModule::RegisterCameraObservers(WorldRef world)
    {
        // Register global transform observer, to update global transform when transform changes
        world->observer<const GlobalTransformComponent, const CameraComponent>()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .run(OnCameraTransformChanged_UpdateFrustum);
    }
} // namespace Ame::Ecs