#include <Ecs/Module/Standard.hpp>

#include <Ecs/Module/Math.hpp>
#include <Ecs/Module/Viewport.hpp>
#include <Ecs/Module/Renderable.hpp>

namespace Ame::Ecs::Module
{
    StandardModule::StandardModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<StandardModule>();

        flecsWorld.import <Module::MathModule>();
        flecsWorld.import <Module::ViewportModule>();
        flecsWorld.import <Module::RenderableModule>();
    }
} // namespace Ame::Ecs::Module