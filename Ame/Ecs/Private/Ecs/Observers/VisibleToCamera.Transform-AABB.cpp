#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Tag/VisibleToCamera.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Math/AABB.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

#include <Ecs/World.hpp>

namespace Ame::Ecs::Module
{
    static void OnVisibilityChangedForCamera(
        const Ecs::Entity::Id             eventType,
        Ecs::Entity&                      entity,
        const Component::BaseRenderable&  renderable,
        const Component::AABBTransformed* transformedBox,
        const Ecs::Entity&                cameraEntity,
        const Component::Camera&          camera,
        const Component::CameraFrustum&   cameraFrustum)
    {
        bool isVisible =
            (eventType == flecs::OnSet) &&
            ((renderable.CameraMask & camera.ViewMask) == camera.ViewMask);

        if (isVisible && transformedBox)
        {
            isVisible = cameraFrustum.Frustum.Contains(transformedBox->BoxTransformed) != Geometry::ContainmentType::DISJOINT;
        }

        if (isVisible)
        {
            entity->add<Tag::VisibleToCamera>(*cameraEntity);
        }
        else
        {
            entity->remove<Tag::VisibleToCamera>(*cameraEntity);
        }
    }

    //

    void RenderableModule::RegisterVisbilityObserver(
        flecs::world& flecsWorld)
    {
        Ecs::WorldRef world(flecsWorld.get_world());

        //

        m_CameraQuery =
            world.CreateQuery<const Component::Camera,
                              const Component::CameraFrustum>()
                .build();

        //

        world.CreateObserver<const Component::BaseRenderable,
                             const Component::AABBTransformed>()
            .term<const Component::AABBTransformed>() // bounding box is not required
            .optional()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .iter([this](Iterator&                         iter,
                         const Component::BaseRenderable*  renderables,
                         const Component::AABBTransformed* transformedBoxes)
                  { OnVisbilityChanged(iter, renderables, transformedBoxes); });

        world.CreateObserver<
                 const Component::Camera,
                 const Component::CameraFrustum>()
            .event(flecs::OnSet)
            .event(flecs::OnRemove)
            .yield_existing()
            .iter([this](Iterator& iter, const Component::Camera* cameras, const Component::CameraFrustum* cameraFrustums)
                  { OnCameraAddRemove(iter, cameras, cameraFrustums); });
    }

    //

    void RenderableModule::OnVisbilityChanged(
        Iterator&                         entityIter,
        const Component::BaseRenderable*  renderables,
        const Component::AABBTransformed* transformedBoxes)
    {
        // if bounding box is not available, we will only check camera mask
        for (auto i : entityIter)
        {
            Ecs::Entity entity(entityIter.entity(i));

            auto& renderable     = renderables[i];
            auto  transformedBox = transformedBoxes ? &transformedBoxes[i] : nullptr;

            m_CameraQuery->iter(
                [&](Iterator&                       cameraIter,
                    const Component::Camera*        cameras,
                    const Component::CameraFrustum* cameraFrustums)
                {
                    for (auto j : cameraIter)
                    {
                        Ecs::Entity cameraEntity(cameraIter.entity(j));

                        auto& camera        = cameras[j];
                        auto& cameraFrustum = cameraFrustums[j];

                        OnVisibilityChangedForCamera(entityIter.event(), entity, renderable, transformedBox, cameraEntity, camera, cameraFrustum);
                    }
                });
        }
    }

    void RenderableModule::OnCameraAddRemove(
        Iterator&                       cameraIter,
        const Component::Camera*        cameras,
        const Component::CameraFrustum* cameraFrustums)
    {
        Ecs::WorldRef world(cameraIter.world());

        // New camera was added, refresh the entities visibility tag
        if (cameraIter.event() == flecs::OnSet)
        {
            auto filter =
                world.CreateFilter<const Component::BaseRenderable,
                                   const Component::AABBTransformed>()
                    .term<const Component::AABBTransformed>() // bounding box is not required
                    .optional()
                    .build();

            for (auto i : cameraIter)
            {
                Ecs::Entity cameraEntity(cameraIter.entity(i));

                auto& camera        = cameras[i];
                auto& cameraFrustum = cameraFrustums[i];

                filter
                    .iter(
                        [&](Iterator&                         entityIter,
                            const Component::BaseRenderable*  renderables,
                            const Component::AABBTransformed* transformedBoxes)
                        {
                            for (auto j : entityIter)
                            {
                                Ecs::Entity entity(entityIter.entity(j));

                                auto& renderable     = renderables[j];
                                auto  transformedBox = transformedBoxes ? &transformedBoxes[j] : nullptr;

                                OnVisibilityChangedForCamera(flecs::OnSet, entity, renderable, transformedBox, cameraEntity, camera, cameraFrustum);
                            }
                        });
            }
        }
        else
        {
            for (auto i : cameraIter)
            {
                Ecs::Entity cameraEntity(cameraIter.entity(i));

                // remove all entities that has (VisibleToCamera, cameraEntity) relation
                world.CreateFilter()
                    .with<Tag::VisibleToCamera>(*cameraEntity)
                    .build()
                    .iter(
                        [&](Iterator& entityIter)
                        {
                            for (auto j : entityIter)
                            {
                                Ecs::Entity entity(entityIter.entity(j));
                                entity->remove<Tag::VisibleToCamera>(*cameraEntity);
                            }
                        });
            }
        }
    }

    //

} // namespace Ame::Ecs::Module