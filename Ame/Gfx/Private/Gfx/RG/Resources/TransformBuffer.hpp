#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Math/Transform.hpp>

namespace Ame::Gfx::RG
{
    class TransformBuffer : public Rhi::Util::SlotBasedBuffer<Math::Matrix4x4>
    {
    public:
        struct GpuId
        {
            SlotType Id;

            explicit GpuId(SlotType id = c_InvalidSlot) noexcept :
                Id(id)
            {
            }
        };

    public:
        using SlotBasedBuffer::SlotBasedBuffer;
    };
} // namespace Ame::Gfx::RG