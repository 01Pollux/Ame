#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Module/Renderable2D.hpp>

namespace Ame::Ecs::Module
{
    RenderableModule::RenderableModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<RenderableModule>();

        flecsWorld.component<Component::BaseRenderable>();

        flecsWorld.import <Renderable2DModule>();
    }
} // namespace Ame::Ecs::Module