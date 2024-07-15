#pragma once

#include <Math/Common.hpp>
#include <map>

namespace Ame::Allocator
{
    template<typename Ty>
    concept BuddyTraits = requires {
        typename Ty::index_type;
        typename Ty::size_type;
        typename Ty::offset_map_type;
        typename Ty::size_map_type;

        std::is_integral_v<typename Ty::index_type>;
        std::is_integral_v<typename Ty::size_type>;
    };

    struct BuddyTraits_U64
    {
        using index_type = size_t;
        using size_type  = size_t;

        using offset_map_type = std::map<index_type, size_type>;
        using size_map_type   = std::map<size_t, offset_map_type::const_iterator>;
    };

    struct BuddyTraits_U32
    {
        using index_type = size_t;
        using size_type  = size_t;

        using offset_map_type = std::map<index_type, size_type>;
        using size_map_type   = std::map<size_t, offset_map_type::const_iterator>;
    };

    template<BuddyTraits Ty = BuddyTraits_U64>
    class Buddy
    {
    public:
        using traits_type = Ty;
        using index_type  = typename traits_type::index_type;
        using size_type   = typename traits_type::size_type;

        using offset_map_type = typename traits_type::offset_map_type;
        using size_map_type   = typename traits_type::size_map_type;

    public:
        struct Handle
        {
            index_type Offset = std::numeric_limits<index_type>::max();
            size_type  Size   = 0;

            explicit operator bool() const noexcept
            {
                return Size != 0;
            }

            [[nodiscard]] auto operator<=>(
                const Handle& other) const noexcept = default;
        };

        explicit Buddy(
            size_type size) :
            m_Size(size)
        {
            m_FreeOffsets.emplace(0, size);
            m_FreeSizes.emplace(size, m_FreeOffsets.begin());
        }

        [[nodiscard]] Handle Allocate(
            size_type size,
            size_type alignement = 1)
        {
            size = Math::AlignUp(size, alignement);

            Handle handle;
            auto   iter = m_FreeSizes.lower_bound(size);
            while (iter != m_FreeSizes.end())
            {
                index_type oldOffset   = iter->second->first;
                index_type offset      = Math::AlignUp(oldOffset, alignement);
                size_type  diffOffset  = offset - oldOffset;
                size_type  alignedSize = iter->first - diffOffset;

                if (alignedSize < size)
                {
                    ++iter;
                    continue;
                }

                handle.Size   = size;
                handle.Offset = offset;

                m_FreeOffsets.erase(iter->second);
                m_FreeSizes.erase(iter);

                if (alignedSize != size)
                {
                    size_type newSize  = alignedSize - size;
                    auto      freeIter = m_FreeOffsets.emplace(handle.Offset + size, newSize);
                    m_FreeSizes.emplace(newSize, freeIter.first);
                }

                if (diffOffset)
                {
                    auto freeIter = m_FreeOffsets.emplace(oldOffset, diffOffset);
                    m_FreeSizes.emplace(diffOffset, freeIter.first);
                }
                break;
            }
            return handle;
        }

        void Free(
            Handle handle)
        {
            auto iter = m_FreeOffsets.lower_bound(handle.Offset);
            if (iter != m_FreeOffsets.end())
            {
                if (iter->first == handle.Offset + handle.Size)
                {
                    handle.Size += iter->second;

                    m_FreeSizes.erase(iter->second);
                    iter = m_FreeOffsets.erase(iter);
                }
            }
            if (iter != m_FreeOffsets.begin())
            {
                --iter;
                if (iter->first + iter->second == handle.Offset)
                {
                    handle.Offset = iter->first;
                    handle.Size += iter->second;

                    m_FreeSizes.erase(iter->second);
                    m_FreeOffsets.erase(iter);
                }
            }
            auto freeIter = m_FreeOffsets.emplace(handle.Offset, handle.Size);
            m_FreeSizes.emplace(handle.Size, freeIter.first);
            handle.Size = 0;
        }

        void Grow(
            size_type size)
        {
            auto iter = m_FreeOffsets.rbegin();
            if (iter != m_FreeOffsets.rend())
            {
                index_type oldOffset = iter->first;
                size_type  newSize   = iter->second + size;

                m_FreeSizes.erase(iter->second);
                m_FreeOffsets.erase(std::next(iter).base());

                auto insert = m_FreeOffsets.emplace(oldOffset, newSize).first;
                m_FreeSizes.emplace(newSize, insert);
            }
            else
            {
                auto insert = m_FreeOffsets.emplace(m_Size, size).first;
                m_FreeSizes.emplace(size, insert);
            }
        }

        void Clear()
        {
            m_FreeOffsets.clear();
            m_FreeSizes.clear();

            m_FreeOffsets.emplace(0, m_Size);
            m_FreeSizes.emplace(m_Size, m_FreeOffsets.begin());
        }

        [[nodiscard]] size_type GetMaxSize() const noexcept
        {
            return m_Size;
        }

    private:
        offset_map_type m_FreeOffsets;
        size_map_type   m_FreeSizes;
        size_type       m_Size;
    };
} // namespace Ame::Allocator