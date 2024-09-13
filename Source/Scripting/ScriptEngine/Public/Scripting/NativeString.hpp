#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>
#include <Core/Hash.hpp>

namespace Ame::Scripting
{
    class NativeString
    {
    public:
        NativeString() = default;
        NativeString(const StringView& str) : m_Data(std::make_unique<char[]>(str.size())), m_Size(str.size())
        {
            std::memcpy(m_Data.get(), str.data(), str.size());
        }
        NativeString(const char* data, size_t size) : m_Data(std::make_unique<char[]>(size)), m_Size(size)
        {
            std::memcpy(m_Data.get(), data, size);
        }
        NativeString(const NativeString& other) : m_Data(std::make_unique<char[]>(other.m_Size)), m_Size(other.m_Size)
        {
            std::memcpy(m_Data.get(), other.m_Data.get(), other.m_Size);
        }
        NativeString(NativeString&&) = default;

        NativeString& operator=(const NativeString& other)
        {
            if (this != &other)
            {
                m_Data = std::make_unique<char[]>(other.m_Size);
                m_Size = other.m_Size;
                std::memcpy(m_Data.get(), other.m_Data.get(), other.m_Size);
            }
            return *this;
        }
        NativeString& operator=(NativeString&&) = default;

        ~NativeString() = default;

        [[nodiscard]] const char* data() const
        {
            return m_Data.get();
        }

        [[nodiscard]] size_t size() const
        {
            return m_Size;
        }
        [[nodiscard]] String ToString() const
        {
            return String(m_Data.get(), m_Size);
        }

        [[nodiscard]] size_t GetHash() const noexcept
        {
            size_t hash = 0;
            HashCombine(hash, m_Data.get(), m_Data.get() + m_Size);
            return hash;
        }

    private:
        std::unique_ptr<char[]> m_Data;
        size_t                  m_Size;
    };
} // namespace Ame::Scripting
