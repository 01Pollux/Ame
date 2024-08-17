#include <EcsModule/CoreModule.hpp>
#include <EcsModule/MathModule.hpp>
#include <EcsModule/ViewportModule.hpp>

#include <EcsComponent/Core/EntityTagComponent.hpp>

namespace Ame::Ecs
{
    CoreEcsModule::CoreEcsModule(
        WorldRef world)
    {
        world->module<CoreEcsModule>();

        world->component<EntityTagComponent>("Ame.EntityTagComponent");

        world->import <MathEcsModule>();
        world->import <ViewporEcsModule>();
    }
} // namespace Ame::Ecs