#pragma once

#include <cstdint>

namespace Ame::Rhi
{
    struct MemoryAllocatorDesc
    {
        /// <summary>
        /// Preferred size of a single 'nri::Memory' block.
        /// </summary>
        size_t InitialBlockSize = 64 * 1024 * 1024;

        /// <summary>
        /// Size of the next block to allocate.
        /// </summary>
        size_t NextBlockSize = InitialBlockSize;

        /// <summary>
        /// Maximum size of a single 'nri::Memory' block.
        /// </summary>
        size_t MaxBlockSize = 128 * 1024 * 1024;

        /// <summary>
        /// Growth factor of the next block to allocate.
        /// </summary>
        float GrowthFactor = 0.5f;

        /// <summary>
        /// Maximum number of attempts to grow the memory pool before failing.
        /// </summary>
        uint32_t MaxGrowAttempts = 50;
    };
} // namespace Ame::Rhi