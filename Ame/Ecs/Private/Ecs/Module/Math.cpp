#include <Ecs/Module/Math.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Math/AABB.hpp>

namespace Ame::Ecs::Module
{
    MathModule::MathModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<MathModule>();

        flecsWorld.component<Component::Transform>();
        flecsWorld.component<Component::AABB>();

        flecsWorld.component<Component::AABBTransformed>();

        //

        RegisterTransformObserver(flecsWorld);
    }
} // namespace Ame::Ecs::Module