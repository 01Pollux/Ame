#include <Core/Interface.hpp>
#include <Core/Hash.hpp>

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

    //

    size_t UIdHasher::operator()(
        const UId& id) const
    {
        size_t hash = 0;
        HashCombine(hash, id.Data1);
        HashCombine(hash, id.Data2);
        HashCombine(hash, id.Data3);
        HashCombine(hash, std::begin(id.Data4), std::end(id.Data4));
        return hash;
    }
} // namespace Ame