#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Geometry/AABB.hpp>

namespace Ame::Gfx::RG
{
    class AABBBuffer : public Rhi::Util::SlotBasedBuffer<Geometry::AABB>
    {
    public:
    };
} // namespace Ame::Gfx::RG