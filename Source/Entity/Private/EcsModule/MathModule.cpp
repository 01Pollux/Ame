#include <EcsModule/MathModule.hpp>

#include <EcsComponent/Math/TransformComponent.hpp>
#include <EcsComponent/Math/AABBComponent.hpp>

namespace Ame::Ecs
{
    MathEcsModule::MathEcsModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<MathEcsModule>();

        //

        flecsWorld.component<TransformComponent>("Ame.TransformComponent");
        flecsWorld.component<GlobalTransformComponent>("Ame.GlobalTransformComponent");

        flecsWorld.component<AABBComponent>("Ame.AABBComponent");
        flecsWorld.component<TransformedAABBComponent>("Ame.TransformedAABBComponent");

        //

        RegisterTransformObservers(flecsWorld);
    }
} // namespace Ame::Ecs