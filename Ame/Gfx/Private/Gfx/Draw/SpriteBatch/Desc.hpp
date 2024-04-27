#pragma once

#include <Rhi/Core.hpp>

namespace Ame::Gfx
{
    struct SpriteBatchDesc
    {
        Ref<Rhi::Device> Device;
        uint32_t         EstimatedIndexBufferSize  = 0xFFFF;
        uint32_t         EstimatedVertexBufferSize = 0xFFFF;
    };
} // namespace Ame::Gfx