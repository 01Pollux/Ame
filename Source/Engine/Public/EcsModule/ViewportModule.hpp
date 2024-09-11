#pragma once

#include <Ecs/World.hpp>
#include <Core/Signal.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// CameraComponent
    /// CameraOutputComponent
    /// CameraFrustumComponent
    /// </summary>
    class AME_ENGINE_API ViewporEcsModule
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