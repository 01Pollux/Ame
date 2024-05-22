#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>
#include <Math/Transform.hpp>

namespace Ame::Gfx::RG
{
    class TransformBuffer : public Rhi::Util::SlotBasedBuffer<Math::Matrix4x4>
    {
    public:
        using SlotBasedBuffer::SlotBasedBuffer;
    };
} // namespace Ame::Gfx::RG