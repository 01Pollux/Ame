#include <EcsModule/MathModule.hpp>

#include <EcsComponent/Math/TransformComponent.hpp>
#include <EcsComponent/Math/AABBComponent.hpp>

namespace Ame::Ecs
{
    MathEcsModule::MathEcsModule(
        WorldRef world)
    {
        world->module<MathEcsModule>();

        //

        world->component<TransformComponent>("Ame.TransformComponent");
        world->component<GlobalTransformComponent>("Ame.GlobalTransformComponent");

        world->component<AABBComponent>("Ame.AABBComponent");
        world->component<TransformedAABBComponent>("Ame.TransformedAABBComponent");

        //

        RegisterTransformObservers(world);
    }
} // namespace Ame::Ecs