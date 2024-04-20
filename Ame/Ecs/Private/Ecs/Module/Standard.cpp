#include <Ecs/Module/Standard.hpp>

#include <Ecs/Component/StdComponent.hpp>

namespace Ame::Ecs::Module
{
    Standard::Standard(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<Standard>();

        FlecsWorld.component<Component::Std::AmeEntityTag>();
    }
} // namespace Ame::Ecs::Module