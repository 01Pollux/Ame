#include <Gfx/Ecs/Modules.hpp>
#include <Gfx/RenderGraph/Buffers/TransformBuffer.hpp>

namespace Ame::Gfx
{
    RGModule::RGModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<RGModule>();

        flecsWorld.component<TransformBuffer::GpuId>();
    }
} // namespace Ame::Gfx