#include <Core/Allocator.hpp>
#include "Allocator.hpp"

namespace Ame::Rhi
{
    void* Ame::Rhi::NriAllocatorCallbackInterface::Allocate(
        void*,
        size_t Size,
        size_t Alignment)
    {
        return mi_aligned_alloc(Alignment, Size);
    }

    void* NriAllocatorCallbackInterface::Reallocate(
        void*,
        void*  Memory,
        size_t Size,
        size_t Alignment)
    {
        return mi_realloc_aligned(Memory, Size, Alignment);
    }

    void NriAllocatorCallbackInterface::Free(
        void*,
        void* Memory)
    {
        mi_free(Memory);
    }
} // namespace Ame::Rhi