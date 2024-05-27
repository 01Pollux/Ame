#include <Ecs/Module/Viewport.hpp>

#include <Ecs/Component/Viewport/Camera.hpp>
#include <Ecs/Component/Viewport/CameraOutput.hpp>

namespace Ame::Ecs::Module
{
    ViewportModule::ViewportModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<ViewportModule>();

        FlecsWorld.component<Component::Camera>();
        FlecsWorld.component<Component::CameraOutput>();
    }
} // namespace Ame::Ecs::Module