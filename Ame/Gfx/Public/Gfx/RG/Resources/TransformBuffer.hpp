#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Math/Transform.hpp>

namespace Ame::Gfx::RG
{
    class TransformBuffer : public Rhi::Util::SlotBasedBuffer<Math::TransformMatrix>
    {
    public:
    };
} // namespace Ame::Gfx::RG