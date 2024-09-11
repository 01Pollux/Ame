#pragma once

#include <Ecs/World.hpp>
#include <Core/Signal.hpp>

namespace Ame::Ecs
{
    class ViewporEcsModule
    {
    public:
        ViewporEcsModule(WorldRef world);

    private:
        void RegisterCameraObservers(WorldRef world);

        void RegisterSignals(WorldRef world);

    private:
        Signals::ScopedConnection m_WindowSizeChangedConnection;
    };
} // namespace Ame::Ecs