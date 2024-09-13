#pragma once

#include <Scripting/Types/NativeArray.hpp>

namespace Ame::Scripting
{
    class NativeString : public NativeArray<char>
    {
    public:
        NativeString() = default;
        NativeString(const StringView& str) : NativeArray(str.size())
        {
            std::memcpy(data(), str.data(), str.size());
        }
        NativeString(const char* str, size_t size) : NativeArray(size)
        {
            std::memcpy(data(), str, size);
        }
        NativeString(const NativeString& other) : NativeArray(other.size())
        {
            std::memcpy(data(), other.data(), other.size_bytes());
        }

        NativeString& operator=(const NativeString&) = default;
        NativeString& operator=(NativeString&&)      = default;
        NativeString(NativeString&&)                 = default;
        ~NativeString()                              = default;

        [[nodiscard]] const char* c_str() const
        {
            return data();
        }

        [[nodiscard]] String str() const
        {
            return String(data(), size());
        }
    };
} // namespace Ame::Scripting
