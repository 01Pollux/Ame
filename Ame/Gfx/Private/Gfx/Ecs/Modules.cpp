#include <Gfx/Ecs/Modules.hpp>
#include <Gfx/RenderGraph/Buffers/TransformBuffer.hpp>
#include <Gfx/RenderGraph/Buffers/AABBBuffer.hpp>
#include <Gfx/RenderGraph/Buffers/InstanceBuffer.hpp>

namespace Ame::Gfx
{
    RGModule::RGModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<RGModule>();

        flecsWorld.component<TransformBuffer::GpuId>("TransformBuffer.GpuId");
        flecsWorld.component<AABBBuffer::GpuId>("AABBBuffer.GpuId");
        flecsWorld.component<InstanceBuffer::GpuId>("RenderInstance.GpuId");
    }
} // namespace Ame::Gfx