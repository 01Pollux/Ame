#include <EcsModule/CoreModule.hpp>
#include <EcsModule/MathModule.hpp>

#include <EcsComponent/Core/EntityTagComponent.hpp>

namespace Ame::Ecs
{
    CoreEcsModule::CoreEcsModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<CoreEcsModule>();

        flecsWorld.component<EntityTagComponent>("Ame.EntityTagComponent");

        flecsWorld.import <MathEcsModule>();
    }
} // namespace Ame::Ecs