#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Gfx/RG/Resources/RenderInstance.hpp>

namespace Ame::Gfx::RG
{
    class DynamicInstanceBuffer : public Rhi::Util::SlotBasedBuffer<RenderInstance>
    {
    public:
        using SlotBasedBuffer::SlotBasedBuffer;
    };
} // namespace Ame::Gfx::RG