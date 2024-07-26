#include <Ecs/Module/Viewport.hpp>

#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>

namespace Ame::Ecs::Module
{
    ViewportModule::ViewportModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<ViewportModule>();

        flecsWorld.component<Component::Camera>();
        flecsWorld.component<Component::CameraFrustum>();
        flecsWorld.component<Component::CameraOutput>();

        //

        RegisterTransformObserver(flecsWorld);
    }

} // namespace Ame::Ecs::Module