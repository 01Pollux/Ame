#pragma once

#include <Core/Ame.hpp>
#include "../Nri/Nri.hpp"

namespace Ame::Rhi
{
    struct NriAllocatorCallbackInterface
    {
        static void* Allocate(
            void*,
            size_t Size,
            size_t Alignment);

        static void* Reallocate(
            void*,
            void*  memory,
            size_t size,
            size_t alignment);

        static void Free(
            void*,
            void* memory);
    };
} // namespace Ame::Rhi