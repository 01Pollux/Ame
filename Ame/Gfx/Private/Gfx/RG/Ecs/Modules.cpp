#include <Gfx/RG/Ecs/Modules.hpp>

#include <Gfx/RG/Resources/TransformBuffer.hpp>

namespace Ame::Gfx::RG
{
    RGModule::RGModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<RGModule>();

        flecsWorld.component<TransformBuffer::GpuId>();
    }
} // namespace Ame::Gfx::RG