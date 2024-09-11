#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// RuntimeSceneComponent
    /// SceneEntityPairComponent
    /// ActiveSceneEntityPairComponent
    /// ActiveSceneEntityTag
    /// </summary>
    class AME_ENGINE_API SceneEcsModule
    {
    public:
        SceneEcsModule(WorldRef world);

    private:
        void RegisterSceneObservers(WorldRef world);
    };
} // namespace Ame::Ecs