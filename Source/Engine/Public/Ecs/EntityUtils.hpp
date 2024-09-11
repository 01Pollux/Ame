#pragma once

#include <Ecs/Entity.hpp>

namespace Ame::Ecs
{
    struct EntityUtils
    {
        /// <summary>
        /// Get the scene root entity of the entity
        /// </summary>
        [[nodiscard]] static Entity GetSceneRoot(const Entity& entity);

        /// <summary>
        /// Clone an entity and its children if needed
        /// if the entity has scene root, the new entity will have the same scene root
        /// if the parent is not set, the parent will be the scene root entity if it exists
        /// </summary>
        static Entity Clone(const Entity& entity, Entity newParent = {}, bool deepClone = true);

        /// <summary>
        /// Clone an entity and its children if needed
        /// if the parent is not set, the parent will be the scene root entity if it exists
        /// </summary>
        static Entity CloneTo(const Entity& sceneRoot, const Entity& entity, Entity newParent = {},
                              bool deepClone = true);
    };
} // namespace Ame::Ecs