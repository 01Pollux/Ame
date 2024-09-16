#pragma once

namespace Ame
{
    template<typename Ty> struct StlAllocator
    {
        using value_type                  = Ty;
        static constexpr size_t alignment = alignof(Ty);

        StlAllocator() noexcept = default;
        template<typename OTy> constexpr StlAllocator(const StlAllocator<OTy>&) noexcept
        {
        }

        [[nodiscard]] Ty* allocate(size_t n) const
        {
            if (n == 0)
            {
                return nullptr;
            }
            if (auto p = static_cast<Ty*>(mi_malloc_aligned(n * sizeof(Ty), alignment)))
            {
                return p;
            }
            throw std::bad_alloc();
        }

        void deallocate(Ty* p, size_t) const noexcept
        {
            mi_free_aligned(p, alignment);
        }
        [[nodiscard]] bool operator==(const StlAllocator&) const noexcept
        {
            return true;
        }
        [[nodiscard]] bool operator!=(const StlAllocator&) const noexcept
        {
            return false;
        }
    };
} // namespace Ame
