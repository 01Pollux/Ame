#include <Ecs/Module/Standard.hpp>

#include <Ecs/Component/StdComponent.hpp>

#include <Ecs/Module/Math.hpp>
#include <Ecs/Module/Geometry2D.hpp>

namespace Ame::Ecs::Module
{
    StandardModule::StandardModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<StandardModule>();

        FlecsWorld.component<Component::Std::AmeEntityTag>();

        FlecsWorld.import <Module::MathModule>();
        FlecsWorld.import <Module::Geometry2DModule>();
    }
} // namespace Ame::Ecs::Module