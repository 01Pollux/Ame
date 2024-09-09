#pragma once

namespace Ame::Ecs
{
    /// <summary>
    /// pair: [SceneEntityComponent, Entity] # entity
    /// entity: Mark the entity as part of a scene
    /// </summary>
    struct SceneEntityPairComponent
    {
    };
    
    /// <summary>
    /// pair: [SceneEntityComponent, Entity] # world
    /// world: Mark the scene as active
    /// </summary>
    struct ActiveSceneEntityPairComponent
    {
    };

    /// <summary>
    /// Tag automatically added to the entities that are part of an active scene
    /// </summary>
    struct ActiveSceneEntityTag
    {
    };
} // namespace Ame::Ecs