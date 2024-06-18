#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Geometry/AABB.hpp>

namespace Ame::Gfx
{
    class AABBBuffer : public Rhi::Util::SlotBasedBuffer<Geometry::AABB>
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
} // namespace Ame::Gfx