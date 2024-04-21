#include <Ecs/Module/Math.hpp>

#include <Ecs/Component/Math/Transform.hpp>

namespace Ame::Ecs::Module
{
    MathModule::MathModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<MathModule>();

        FlecsWorld.component<Component::Transform>();
    }
} // namespace Ame::Ecs::Module