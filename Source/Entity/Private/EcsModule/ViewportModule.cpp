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

        world->component<CameraComponent>("Ame.CameraComponent");
        world->component<CameraOutputComponent>("Ame.CameraOutputComponent");
        world->component<CameraFrustumComponent>("Ame.CameraFrustumComponent");

        //

        RegisterCameraObservers(world);

        //

        RegisterSignals(world);
    }
} // namespace Ame::Ecs