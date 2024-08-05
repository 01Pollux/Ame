#include <EcsModule/ViewportModule.hpp>

#include <EcsComponent/Viewport/Camera.hpp>
#include <EcsComponent/Viewport/CameraOutput.hpp>

namespace Ame::Ecs
{
    ViewporEcstModule::ViewporEcstModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<ViewporEcstModule>();

        flecsWorld.component<CameraComponent>("Ame.CameraComponent");
        flecsWorld.component<CameraOutputComponent>("Ame.CameraOutputComponent");
    }
} // namespace Ame::Ecs