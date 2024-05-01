#pragma once

#include <Gfx/Draw/Buffers/SlotBasedBuffer.hpp>
#include <Gfx/Ecs/Component/RenderInstance.hpp>

namespace Ame::Gfx::Draw::Buffers
{
    using InstanceBuffer = SlotBasedBuffer<Ecs::Gfx::Component::RenderInstance>;
} // namespace Ame::Gfx::Draw::Buffers