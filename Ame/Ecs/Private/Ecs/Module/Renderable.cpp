#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Module/Renderable2D.hpp>

namespace Ame::Ecs::Module
{
    RenderableModule::RenderableModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<RenderableModule>();

        FlecsWorld.component<Component::BaseRenderable>();
        FlecsWorld.component<Component::BaseRenderableTag>();

        FlecsWorld.import <Renderable2DModule>();
    }
} // namespace Ame::Ecs::Module