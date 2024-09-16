#pragma once

#include <Scripting/Types/NativeSpan.hpp>
#include <Scripting/Types/Converter.hpp>
#include <Core/String.hpp>

namespace Ame::Scripting
{
    class NativeString
    {
    public:
        using char_type       = char;
        using std_string_type = std::basic_string<char_type>;
        using std_view_type   = std::basic_string_view<char_type>;

        NativeString() = default;
        template<size_t N>
        NativeString(const char_type (&str)[N]) : m_Data(std::make_unique<char_type[]>(N)), m_Size(N - 1)
        {
            std::copy(str, str + N - 1, data());
        }
        NativeString(const std_view_type& str) :
            m_Data(std::make_unique<char_type[]>(str.size() + 1)), m_Size(str.size())
        {
            std::copy(str.begin(), str.end(), data());
        }
        NativeString(const char_type* str, size_t count) :
            m_Data(std::make_unique<char_type[]>(count + 1)), m_Size(count)
        {
            std::copy(str, str + count, data());
        }
        NativeString(const NativeString& other) :
            m_Data(std::make_unique<char_type[]>(other.size() + 1)), m_Size(other.size())
        {
            std::copy(other.begin(), other.end(), data());
        }

        NativeString& operator=(const NativeString& other)
        {
            if (this != &other)
            {
                if (m_Size != other.size())
                {
                    m_Data = std::make_unique<char_type[]>(other.size() + 1);
                    m_Size = other.size();
                }
                std::copy(other.begin(), other.end(), data());
            }
            return *this;
        }

        NativeString& operator=(NativeString&&) = default;
        NativeString(NativeString&&)            = default;
        ~NativeString()                         = default;

    public:
        [[nodiscard]] std_string_type str() const
        {
            return std_string_type(data(), size());
        }
        [[nodiscard]] std_view_type view() const
        {
            return std_view_type(data(), size());
        }

        [[nodiscard]] const char* c_str() const
        {
            return data();
        }
        [[nodiscard]] char_type* data() const noexcept
        {
            return m_Data.get();
        }

        const char_type* begin() const noexcept
        {
            return m_Data.get();
        }
        char_type* begin() noexcept
        {
            return m_Data.get();
        }
        const char_type* end() const noexcept
        {
            return m_Data.get() + m_Size;
        }
        char_type* end() noexcept
        {
            return m_Data.get() + m_Size;
        }
        const char_type* cbegin() const noexcept
        {
            return m_Data.get();
        }
        const char_type* cend() const noexcept
        {
            return m_Data.get() + m_Size;
        }

        char_type& operator[](size_t index)
        {
            return m_Data[index];
        }
        const char_type& operator[](size_t index) const
        {
            return m_Data[index];
        }

        void resize(size_t size, bool keepContents = true)
        {
            if (size == m_Size)
            {
                return;
            }

            auto newData = std::make_unique<char_type[]>(size + 1);
            if (keepContents)
            {
                std::copy(begin(), begin() + std::min(size, m_Size), newData.get());
            }
            m_Data = std::move(newData);
            m_Size = size;
        }

        [[nodiscard]] NativeSpan<char_type> span() const noexcept
        {
            return NativeSpan<char_type>(m_Data.get(), m_Size);
        }
        [[nodiscard]] NativeSpan<char_type> span(size_t offset) const noexcept
        {
            return NativeSpan<char_type>(m_Data.get() + offset, m_Size - offset);
        }
        [[nodiscard]] NativeSpan<char_type> span(size_t offset, size_t count) const noexcept
        {
            return NativeSpan<char_type>(m_Data.get() + offset, count);
        }

        [[nodiscard]] size_t size() const noexcept
        {
            return m_Size;
        }
        [[nodiscard]] size_t size_bytes() const noexcept
        {
            return m_Size * sizeof(char_type);
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
        std::unique_ptr<char_type[]> m_Data;
        size_t                       m_Size;
    };

    template<StringType Ty> struct NativeConverter<Ty>
    {
        static NativeString Wrap(const Ty& value)
        {
            return NativeString(StringView(value));
        }
    };
} // namespace Ame::Scripting
