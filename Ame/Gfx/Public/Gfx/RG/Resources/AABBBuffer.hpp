#pragma once

#include <Gfx/Draw/Buffers/SlotBasedBuffer.hpp>
#include <Geometry/AABB.hpp>

namespace Ame::Gfx::Draw::Buffers
{
    using AABBBuffer = SlotBasedBuffer<Geometry::AABB>;
} // namespace Ame::Gfx::Draw::Buffers