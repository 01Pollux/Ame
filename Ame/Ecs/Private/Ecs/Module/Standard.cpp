#include <Ecs/Module/Standard.hpp>

#include <Ecs/Component/StdComponent.hpp>

#include <Ecs/Module/Math.hpp>
#include <Ecs/Module/Renderable.hpp>

namespace Ame::Ecs::Module
{
    StandardModule::StandardModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<StandardModule>();

        FlecsWorld.component<Component::Std::AmeEntityTag>();

        FlecsWorld.import <Module::MathModule>();
        FlecsWorld.import <Module::RenderableModule>();
    }
} // namespace Ame::Ecs::Module