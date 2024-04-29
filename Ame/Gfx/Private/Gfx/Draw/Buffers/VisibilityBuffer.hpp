#pragma once

#include <Gfx/Draw/Buffers/SlotBasedBuffer.hpp>

namespace Ame::Gfx::Draw::Buffers
{
    using VisibilityBuffer = SlotBasedBuffer<uint32_t>;
} // namespace Ame::Gfx::Draw::Buffers