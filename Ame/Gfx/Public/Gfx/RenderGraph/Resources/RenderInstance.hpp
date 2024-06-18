#pragma once

#include <limits>
#include <cstdint>

namespace Ame::Gfx
{
    struct RenderInstance
    {
    public:
        struct GpuId
        {
            uint32_t Id;

            explicit GpuId(uint32_t id = std::numeric_limits<uint32_t>::max()) noexcept :
                Id(id)
            {
            }
        };

        uint32_t TransformId = std::numeric_limits<uint32_t>::max();
        uint32_t AABBId      = std::numeric_limits<uint32_t>::max();
    };
} // namespace Ame::Gfx