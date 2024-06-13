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
        using SlotBasedBuffer::SlotBasedBuffer;
    };
} // namespace Ame::Gfx