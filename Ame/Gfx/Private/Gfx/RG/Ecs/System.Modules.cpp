#include <Gfx/RG/Ecs/Modules.hpp>

namespace Ame::Gfx::RG
{
    void EcsSystemHooks::RegisterModules(
        Ecs::World& world)
    {
        world.ImportModule<RGModule>();
    }
} // namespace Ame::Gfx::RG