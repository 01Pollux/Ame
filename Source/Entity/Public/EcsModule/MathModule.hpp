#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    class MathEcsModule
    {
    public:
        MathEcsModule(
            WorldRef world);

    private:
        void RegisterTransformObservers(
            WorldRef world);
    };
} // namespace Ame::Ecs