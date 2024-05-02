#include <Ecs/Module/Standard.hpp>

#include <Ecs/Module/Math.hpp>
#include <Ecs/Module/Viewport.hpp>
#include <Ecs/Module/Renderable.hpp>

namespace Ame::Ecs::Module
{
    StandardModule::StandardModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<StandardModule>();

        FlecsWorld.import <Module::MathModule>();
        FlecsWorld.import <Module::ViewportModule>();
        FlecsWorld.import <Module::RenderableModule>();
    }
} // namespace Ame::Ecs::Module