#include <EcsModule/MathModule.hpp>

#include <EcsComponent/Math/Transform.hpp>
#include <EcsComponent/Math/AABB.hpp>

namespace Ame::Ecs
{
    MathEcsModule::MathEcsModule(WorldRef world)
    {
        world->module<MathEcsModule>();

        //

        world->component<TransformComponent>();
        world->component<GlobalTransformComponent>();

        world->component<AABBComponent>();
        world->component<TransformedAABBComponent>();

        //

        RegisterTransformObservers(world);
    }
} // namespace Ame::Ecs