#pragma once

#include <Rhi/Util/SlotBasedBuffer.hpp>

namespace Ame::Gfx
{
    struct EcsSystemDesc
    {
        Rhi::Util::SlotBasedBufferDesc TransformBufferDesc;
        Rhi::Util::SlotBasedBufferDesc AABBBufferDesc;
    };
} // namespace Ame::Gfx