#include <Ecs/Module/Viewport.hpp>

#include <Ecs/Component/Viewport/Camera.hpp>

namespace Ame::Ecs::Module
{
    ViewportModule::ViewportModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<ViewportModule>();

        FlecsWorld.component<Component::Camera>();
    }
} // namespace Ame::Ecs::Module