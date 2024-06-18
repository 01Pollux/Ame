#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Gfx/RenderGraph/Resources/RenderInstance.hpp>

namespace Ame::Gfx
{
    class InstanceBuffer : public Rhi::Util::SlotBasedBuffer<RenderInstance>
    {
    public:
        using GpuId = typename DataType::GpuId;

        using SlotBasedBuffer::SlotBasedBuffer;
    };
} // namespace Ame::Gfx