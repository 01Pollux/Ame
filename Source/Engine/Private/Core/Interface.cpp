#include <Core/Interface.hpp>
#include <mimalloc.h>

namespace Ame
{
    ObjectMemoryAllocator& ObjectMemoryAllocator::Instance()
    {
        static ObjectMemoryAllocator allocator;
        return allocator;
    }

    void* ObjectMemoryAllocator::Allocate(size_t size, const char*, const char*, const int)
    {
        return mi_malloc(size);
    }

    void ObjectMemoryAllocator::Free(void* ptr)
    {
        mi_free(ptr);
    }
} // namespace Ame