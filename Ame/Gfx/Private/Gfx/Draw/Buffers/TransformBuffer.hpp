#pragma once

#include <Gfx/Draw/Buffers/SlotBasedBuffer.hpp>
#include <Math/Transform.hpp>

namespace Ame::Gfx::Draw::Buffers
{
    using TransformBuffer = SlotBasedBuffer<Math::TransformMatrix>;
} // namespace Ame::Gfx::Draw::Buffers