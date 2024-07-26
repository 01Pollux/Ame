#pragma once

#include <Ecs/Iterable.hpp>
#include <Ecs/Query.hpp>

namespace Ame::Ecs::Component
{
    struct BaseRenderable;
    struct AABBTransformed;
    struct Camera;
    struct CameraFrustum;
} // namespace Ame::Ecs::Component

namespace Ame::Ecs::Module
{
    class RenderableModule
    {
    private:
        using CameraFrustumQuery = Ecs::UniqueQuery<const Component::Camera,
                                                    const Component::CameraFrustum>;

    public:
        RenderableModule(
            flecs::world& flecsWorld);

    private:
        /// <summary>
        /// Registers the observer for visibility changes
        /// </summary>
        void RegisterVisbilityObserver(
            flecs::world& flecsWorld);

        /// <summary>
        /// Called whenever entity's any of these changed
        /// - Transform
        /// - bounding box (if exists)
        /// - BaseRenderable::ViewMask
        /// Adds or remove 'VisibleToCamera' tag
        /// </summary>
        void OnVisbilityChanged(
            Iterator&                         entityIter,
            const Component::BaseRenderable*  renderables,
            const Component::AABBTransformed* transformedBoxes);

        /// <summary>
        /// Called whenever a camera component is added or removed
        /// </summary>
        void OnCameraAddRemove(
            Iterator&                       cameraIter,
            const Component::Camera*        cameras,
            const Component::CameraFrustum* cameraFrustums);

    private:
        CameraFrustumQuery m_CameraQuery;
    };
} // namespace Ame::Ecs::Module