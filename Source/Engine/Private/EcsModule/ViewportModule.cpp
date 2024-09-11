#include <EcsModule/ViewportModule.hpp>

#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>
#include <EcsComponent/Viewport/CameraFrustum.hpp>

namespace Ame::Ecs
{
    ViewporEcsModule::ViewporEcsModule(WorldRef world)
    {
        world->module<ViewporEcsModule>();

        //

        world->component<CameraComponent>();
        world->component<CameraOutputComponent>();
        world->component<CameraFrustumComponent>();

        //

        RegisterCameraObservers(world);

        //

        RegisterSignals(world);
    }
} // namespace Ame::Ecs