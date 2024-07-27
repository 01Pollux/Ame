#include <Core/Interface.hpp>
#include <mimalloc.h>

namespace Ame
{
    static inline ObjectMemoryAllocator s_ObjectMemoryAllocator;

    ObjectMemoryAllocator& ObjectMemoryAllocator::Instance()
    {
        return s_ObjectMemoryAllocator;
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