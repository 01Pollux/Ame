#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    class SceneEcsModule
    {
    public:
        SceneEcsModule(WorldRef world);

    private:
        void RegisterSceneObservers(WorldRef world);
    };
} // namespace Ame::Ecs