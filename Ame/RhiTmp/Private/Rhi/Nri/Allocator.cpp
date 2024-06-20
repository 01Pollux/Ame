#include <Rhi/Nri/Allocator.hpp>

namespace Ame::Rhi
{
    void* NriAllocatorCallbackInterface::Allocate(
        void*,
        size_t size,
        size_t alignment)
    {
        return mi_aligned_alloc(alignment, size);
    }

    void* NriAllocatorCallbackInterface::Reallocate(
        void*,
        void*  memory,
        size_t size,
        size_t alignment)
    {
        return mi_realloc_aligned(memory, size, alignment);
    }

    void NriAllocatorCallbackInterface::Free(
        void*,
        void* memory)
    {
        mi_free(memory);
    }
} // namespace Ame::Rhi