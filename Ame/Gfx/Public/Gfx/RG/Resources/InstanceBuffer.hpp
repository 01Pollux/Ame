#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Gfx/Ecs/Component/RenderInstance.hpp>

namespace Ame::Gfx::RG
{
    class InstanceBuffer : public Rhi::Util::SlotBasedBuffer<Ecs::Gfx::Component::RenderInstance>
    {
    public:
    };
} // namespace Ame::Gfx::RG