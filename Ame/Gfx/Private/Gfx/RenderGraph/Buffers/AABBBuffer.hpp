#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Geometry/AABB.hpp>

namespace Ame::Gfx
{
    struct AlignedAABB
    {
        Geometry::AABB Box;
        uint32_t       Padding[2];
    };

    class AABBBuffer : public Rhi::Util::SlotBasedBuffer<AlignedAABB>
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