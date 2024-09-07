#pragma once

namespace Ame::Ecs
{
    /// <summary>
    /// pair: [SceneEntityComponent, Entity] # world,entity
    /// world: Mark the scene as active
    /// entity: Mark the entity as part of a scene
    /// </summary>
    struct SceneEntityPairComponent
    {
    };

    /// <summary>
    /// Tag automatically added to the entities that are part of an active scene
    /// </summary>
    struct ActiveSceneEntityTag
    {
    };
} // namespace Ame::Ecs