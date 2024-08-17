#pragma once

#include <Ecs/Iterable.hpp>

namespace Ame::Ecs
{
    class ViewporEcstModule
    {
    public:
        ViewporEcstModule(
            flecs::world& flecsWorld);

    private:
        void RegisterCameraObservers(
            flecs::world& flecsWorld);
    };
} // namespace Ame::Ecs