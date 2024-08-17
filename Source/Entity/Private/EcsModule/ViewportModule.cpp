#include <EcsModule/ViewportModule.hpp>

#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>
#include <EcsComponent/Viewport/CameraFrustum.hpp>

namespace Ame::Ecs
{
    ViewporEcstModule::ViewporEcstModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<ViewporEcstModule>();

        //

        flecsWorld.component<CameraComponent>("Ame.CameraComponent");
        flecsWorld.component<CameraOutputComponent>("Ame.CameraOutputComponent");
        flecsWorld.component<CameraFrustumComponent>("Ame.CameraFrustumComponent");

        //

        RegisterCameraObservers(flecsWorld);
    }
} // namespace Ame::Ecs