#include <Ecs/Module/Math.hpp>

#include <Ecs/Component/Math/Transform.hpp>

namespace Ame::Ecs::Module
{
    MathModule::MathModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<MathModule>();

        flecsWorld.component<Component::Transform>();
    }
} // namespace Ame::Ecs::Module