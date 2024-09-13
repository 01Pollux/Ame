#pragma once

#include <span>
#include <Core/Hash.hpp>

namespace Ame::Scripting
{
    template<typename Ty> class NativeSpan
    {
    public:
        NativeSpan() = default;
        NativeSpan(Ty* data, size_t size) : m_Data(data), m_Size(size)
        {
        }
        NativeSpan(const NativeSpan& other) : m_Data(other.m_Data), m_Size(other.m_Size)
        {
        }
        NativeSpan(NativeSpan&&) = default;
        NativeSpan& operator=(const NativeSpan& other)
        {
            if (this != &other)
            {
                m_Data = other.m_Data;
                m_Size = other.m_Size;
            }
            return *this;
        }
        NativeSpan& operator=(NativeSpan&&) = default;
        ~NativeSpan()                       = default;

        [[nodiscard]] Ty* data() const
        {
            return m_Data;
        }
        [[nodiscard]] auto begin() const
        {
            return m_Data;
        }
        [[nodiscard]] auto end() const
        {
            return m_Data + m_Size;
        }
        [[nodiscard]] auto cbegin() const
        {
            return m_Data;
        }
        [[nodiscard]] auto cend() const
        {
            return m_Data + m_Size;
        }

        [[nodiscard]] Ty& operator[](size_t index)
        {
            return m_Data[index];
        }
        [[nodiscard]] const Ty& operator[](size_t index) const
        {
            return m_Data[index];
        }

        [[nodiscard]] NativeSpan<Ty> span() const
        {
            return NativeSpan<Ty>(m_Data, m_Size);
        }
        [[nodiscard]] NativeSpan<Ty> span(size_t offset) const
        {
            return NativeSpan<Ty>(m_Data + offset, m_Size - offset);
        }
        [[nodiscard]] NativeSpan<Ty> span(size_t offset, size_t count) const
        {
            return NativeSpan<Ty>(m_Data + offset, count);
        }

        [[nodiscard]] size_t size() const
        {
            return m_Size;
        }
        [[nodiscard]] size_t size_bytes() const
        {
            return m_Size * sizeof(Ty);
        }
        [[nodiscard]] bool empty() const
        {
            return m_Size == 0;
        }

        [[nodiscard]] size_t hash() const noexcept
        {
            size_t hash = 0;
            HashCombine(hash, begin(), end());
            return hash;
        }

    private:
        Ty*    m_Data = nullptr;
        size_t m_Size = 0;
    };
} // namespace Ame::Scripting
