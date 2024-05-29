#pragma once

#include <Math/Common.hpp>
#include <map>

namespace Ame::Allocator
{
    class Buddy
    {
    public:
        struct Handle
        {
            size_t Offset = std::numeric_limits<size_t>::max();
            size_t Size   = 0;

            operator bool() const noexcept
            {
                return Size != 0;
            }

            [[nodiscard]] auto operator<=>(
				const Handle& other) const noexcept = default;
        };

        explicit Buddy(
            size_t size)
        {
            m_FreeOffsets.emplace(0, size);
            m_FreeSizes.emplace(size, m_FreeOffsets.begin());
        }

        [[nodiscard]] Handle Allocate(
            size_t size,
            size_t alignement = 1)
        {
            size = Math::AlignUp(size, alignement);

            Handle handle;
            auto   iter = m_FreeSizes.lower_bound(size);
            while (iter != m_FreeSizes.end())
            {
                size_t oldOffset   = iter->second->first;
                size_t offset      = Math::AlignUp(oldOffset, alignement);
                size_t diffOffset  = offset - oldOffset;
                size_t alignedSize = iter->first - diffOffset;

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
                    size_t newSize  = alignedSize - size;
                    auto   freeIter = m_FreeOffsets.emplace(handle.Offset + size, newSize);
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

    private:
        using OffsetMapType = std::map<size_t, size_t>;
        using SizeMapType   = std::map<size_t, OffsetMapType::const_iterator>;

        OffsetMapType m_FreeOffsets;
        SizeMapType   m_FreeSizes;
    };
} // namespace Ame::Allocator