#pragma once

#include <memory>
#include <Scripting/Types/NativeSpan.hpp>

namespace Ame::Scripting
{
    template<typename Ty> class NativeArray
    {
    public:
        NativeArray() = default;
        NativeArray(size_t size) : m_Data(std::make_unique<Ty[]>(size)), m_Size(size)
        {
        }
        NativeArray(const NativeArray& other) : m_Data(std::make_unique<Ty[]>(other.m_Size)), m_Size(other.m_Size)
        {
            std::copy(other.begin(), other.end(), m_Data.get());
        }
        NativeArray(std::span<const Ty> span) : m_Data(std::make_unique<Ty[]>(span.size())), m_Size(span.size())
        {
            for (size_t i = 0; i < span.size(); i++)
            {
                m_Data[i] = std::move(span[i]);
            }
        }
        NativeArray(std::span<Ty> span) : m_Data(std::make_unique<Ty[]>(span.size())), m_Size(span.size())
        {
            for (size_t i = 0; i < span.size(); i++)
            {
                m_Data[i] = std::move(span[i]);
            }
        }
        template<typename... Args>
        NativeArray(Args&&... args) : m_Data(std::make_unique<Ty[]>(sizeof...(args))), m_Size(sizeof...(args))
        {
            size_t i = 0;
            ((m_Data[i++] = std::forward<Args>(args)), ...);
        }
        NativeArray(NativeArray&&) = default;
        NativeArray& operator=(const NativeArray& other)
        {
            if (this != &other)
            {
                m_Data = std::make_unique<Ty[]>(other.m_Size);
                m_Size = other.m_Size;
                std::memcpy(m_Data.get(), other.m_Data.get(), other.m_Size * sizeof(Ty));
            }
            return *this;
        }
        NativeArray& operator=(NativeArray&&) = default;
        ~NativeArray()                        = default;

        [[nodiscard]] Ty* data() const noexcept
        {
            return m_Data.get();
        }
        auto begin() const noexcept
        {
            return m_Data.get();
        }
        auto end() const noexcept
        {
            return m_Data.get() + m_Size;
        }
        auto cbegin() const noexcept
        {
            return m_Data.get();
        }
        auto cend() const noexcept
        {
            return m_Data.get() + m_Size;
        }

        Ty& operator[](size_t index)
        {
            return m_Data[index];
        }
        const Ty& operator[](size_t index) const
        {
            return m_Data[index];
        }

        void resize(size_t size, bool keepContents = true)
        {
            auto newData = std::make_unique<Ty[]>(size);
            if (keepContents)
            {
                std::memcpy(newData.get(), m_Data.get(), std::min(m_Size, size) * sizeof(Ty));
            }
            m_Data = std::move(newData);
            m_Size = size;
        }

        [[nodiscard]] NativeSpan<Ty> span() const noexcept
        {
            return NativeSpan<Ty>(m_Data.get(), m_Size);
        }
        [[nodiscard]] NativeSpan<Ty> span(size_t offset) const noexcept
        {
            return NativeSpan<Ty>(m_Data.get() + offset, m_Size - offset);
        }
        [[nodiscard]] NativeSpan<Ty> span(size_t offset, size_t count) const noexcept
        {
            return NativeSpan<Ty>(m_Data.get() + offset, count);
        }

        [[nodiscard]] size_t size() const noexcept
        {
            return m_Size;
        }
        [[nodiscard]] size_t size_bytes() const noexcept
        {
            return m_Size * sizeof(Ty);
        }
        [[nodiscard]] bool empty() const noexcept
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
        std::unique_ptr<Ty[]> m_Data;
        size_t                m_Size = 0;
    };
} // namespace Ame::Scripting
