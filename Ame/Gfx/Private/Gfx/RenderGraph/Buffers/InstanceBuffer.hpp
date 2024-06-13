#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Gfx/RenderGraph/Resources/RenderInstance.hpp>

namespace Ame::Gfx
{
    class DynamicInstanceBuffer : public Rhi::Util::SlotBasedBuffer<RenderInstance>
    {
    public:
        using SlotBasedBuffer::SlotBasedBuffer;
    };
} // namespace Ame::Gfx