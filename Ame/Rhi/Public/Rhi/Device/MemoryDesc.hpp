#pragma once

#include <cstdint>

namespace Ame::Rhi
{
    struct MemoryAllocatorDesc
    {
        /// <summary>
        /// Preferred size of the first block to allocate.
        /// </summary>
        size_t BlockSize = 0;

        /// <summary>
        /// Resource allocation and release is thread-safe.
        /// </summary>
        bool MultiThreaded : 1 = true;
    };
} // namespace Ame::Rhi