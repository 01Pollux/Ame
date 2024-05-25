#pragma once

#include <Core/Ame.hpp>
#include <format>
#include <string>
#include <algorithm>

namespace Ame
{
    using Char       = char;
    using StringView = std::string_view;
    using String     = std::string;

    using WideChar       = wchar_t;
    using WideStringView = std::wstring_view;
    using WideString     = std::wstring;
} // namespace Ame

namespace Ame
{
    namespace Concepts
    {
        /// <summary>
        /// String type concept
        /// </summary>
        template<typename Ty>
        concept StringType = std::is_same_v<Ty, String> || std::is_same_v<Ty, WideString> ||
                             std::is_same_v<Ty, StringView> || std::is_same_v<Ty, WideStringView> ||
                             std::is_same_v<Ty, std::string> || std::is_same_v<Ty, std::wstring> ||
                             std::is_same_v<Ty, std::string_view> || std::is_same_v<Ty, std::wstring_view> ||
                             std::is_same_v<std::remove_cv_t<std::remove_pointer_t<std::decay_t<Ty>>>, Char> ||
                             std::is_same_v<std::remove_cv_t<std::remove_pointer_t<std::decay_t<Ty>>>, WideChar>;
    } // namespace Concepts
} // namespace Ame

namespace Ame::Strings
{
    /// <summary>
    /// Empty string
    /// </summary>
    template<typename Ty>
        requires std::is_same_v<Ty, String> || std::is_same_v<Ty, WideString>
    static Ty c_Empty = {};

    /// <summary>
    /// Transform string to another type
    /// </summary>
    template<typename ToTy, typename FromTy>
    [[nodiscard]] constexpr ToTy To(
        const FromTy& str) noexcept
    {
        // same type
        if constexpr (std::is_same_v<ToTy, FromTy>)
            return str;
        // from const _char* to _string_view
        else if constexpr (std::is_pointer_v<FromTy>)
            return To<ToTy>(std::basic_string_view<std::remove_pointer_t<FromTy>>(str));
        else
        {
            if (str.empty())
                return {};
            // from bigger type to smaller type, eg: wstring to string, u32string to u8string, etc...
            if constexpr (sizeof(typename FromTy::value_type) > sizeof(typename ToTy::value_type))
            {
                ToTy buf(str.size(), 0);
                std::transform(std::begin(str), std::end(str), std::begin(buf), [](const typename FromTy::value_type c)
                               { return static_cast<typename ToTy::value_type>(c); });
                return buf;
            }
            // from smaller type to bigger type, eg: string to wstring, etc...
            else
                return { std::begin(str), std::end(str) };
        }
    }

    /// <summary>
    /// Transform string to another type
    /// </summary>
    template<typename ToTy, typename FromTy, size_t Size>
    [[nodiscard]] constexpr ToTy Transform(
        const FromTy (&str)[Size]) noexcept
    {
        if constexpr (std::is_same_v<typename ToTy::value_type, FromTy>)
            return ToTy{ str };
        else if constexpr (Size < 1)
            return ToTy{};
        else
        {
            ToTy toStr{};
            toStr.reserve(Size);
            std::transform(std::begin(str), std::end(str), std::back_inserter(toStr), [](FromTy c)
                           { return static_cast<typename ToTy::value_type>(c); });
            return toStr;
        }
    }

    //

    /// <summary>
    /// Convert string to lower case
    /// </summary>
    [[nodiscard]] static String ToLower(
        const StringView& str) noexcept
    {
        String lowStr;
        lowStr.reserve(str.size());
        std::ranges::transform(str, std::back_inserter(lowStr), [](Char c)
                               { return static_cast<Char>(std::tolower(c)); });
        return lowStr;
    }

    /// <summary>
    /// Convert string to lower case
    /// </summary>
    [[nodiscard]] static WideString ToLower(
        const WideStringView& str) noexcept
    {
        WideString lowStr;
        lowStr.reserve(str.size());
        std::ranges::transform(str, std::back_inserter(lowStr), [](WideChar c)
                               { return static_cast<WideChar>(std::tolower(c)); });
        return lowStr;
    }

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] static String ToUpper(
        const StringView& str) noexcept
    {
        String upStr;
        upStr.reserve(str.size());
        std::ranges::transform(str, std::back_inserter(upStr), [](Char c)
                               { return static_cast<Char>(std::toupper(c)); });
        return upStr;
    }

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] static WideString ToUpper(
        const WideStringView& str) noexcept
    {
        WideString upStr;
        upStr.reserve(str.size());
        std::ranges::transform(str, std::back_inserter(upStr), [](WideChar c)
                               { return static_cast<WideChar>(std::toupper(c)); });
        return upStr;
    }

    //

    /// <summary>
    /// Replace occurence of a token in a string
    /// </summary>
    static bool Replace(
        String&    str,
        StringView token,
        StringView value) noexcept
    {
        size_t iter = str.find(token.data(), 0, token.size());
        if (iter != str.npos)
        {
            str.replace(iter, token.size(), value);
            return true;
        }
        return false;
    }

    /// <summary>
    /// Replace occurence of a token in a string
    /// </summary>
    static bool Replace(
        WideString&    str,
        WideStringView token,
        WideStringView value) noexcept
    {
        size_t iter = str.find(token.data(), 0, token.size());
        if (iter != str.npos)
        {
            str.replace(iter, token.size(), value);
            return true;
        }
        return false;
    }

    //

    /// <summary>
    /// Replace all occurences of a token in a string
    /// </summary>
    static size_t ReplaceAll(
        String&    str,
        StringView token,
        StringView value) noexcept
    {
        size_t count = 0;
        while (true)
        {
            size_t iter = str.find(token.data(), 0, token.size());
            if (iter != str.npos)
            {
                str.replace(iter, token.size(), value);
                count++;
            }
            else
            {
                break;
            }
        }
        return count;
    }

    /// <summary>
    /// Replace all occurences of a token in a string
    /// </summary>
    static size_t ReplaceAll(
        WideString&    str,
        WideStringView token,
        WideStringView value) noexcept
    {
        size_t count = 0;
        while (true)
        {
            size_t iter = str.find(token.data(), 0, token.size());
            if (iter != str.npos)
            {
                str.replace(iter, token.size(), value);
                count++;
            }
            else
            {
                break;
            }
        }
        return count;
    }

    //

    /// <summary>
    /// Hash a string
    /// </summary>
    template<Concepts::StringType Ty>
    [[nodiscard]] constexpr size_t Hash(
        const Ty& str)
    {
        // https://github.com/elanthis/constexpr-hash-demo/blob/master/test.cpp
        // FNV-1a constants

        constexpr size_t c_HashBasis = 14695981039346656037ULL;
        constexpr size_t c_HashPrime = 1099511628211ULL;

        size_t hash = c_HashBasis;
        for (auto c : std::basic_string_view(str))
        {
            hash ^= c;
            hash *= c_HashPrime;
        }
        return hash;
    }
} // namespace Ame::Strings

namespace Ame
{
    struct StringHash
    {
    public:
        template<Concepts::StringType Ty>
        constexpr StringHash(
            const Ty& str) :
            StringHash(Strings::Hash(str))
        {
        }

        constexpr StringHash(
            size_t hash) :
            m_Hash(hash)
        {
        }

        [[nodiscard]] constexpr operator size_t() const
        {
            return m_Hash;
        }

        constexpr auto operator<=>(const StringHash&) const = default;

    private:
        size_t m_Hash;
    };

    namespace Literals
    {
        [[nodiscard]] constexpr StringHash operator""_hash(
            const char* str,
            size_t      size)
        {
            return Strings::Hash(std::string_view(str, size));
        }

        [[nodiscard]] constexpr StringHash operator""_hash(
            const wchar_t* str,
            size_t         size)
        {
            return Strings::Hash(std::wstring_view(str, size));
        }
    } // namespace Literals
} // namespace Ame

namespace std
{
    template<>
    struct hash<Ame::StringView>
    {
        size_t operator()(const Ame::StringView& str) const
        {
            return Ame::StringHash{ str };
        }
    };

    template<>
    struct hash<Ame::WideStringView>
    {
        size_t operator()(const Ame::WideStringView& str) const
        {
            return Ame::StringHash{ str };
        }
    };

    template<>
    struct hash<Ame::String>
    {
        size_t operator()(const Ame::String& str) const
        {
            return Ame::StringHash{ str };
        }
    };

    template<>
    struct hash<Ame::WideString>
    {
        size_t operator()(const Ame::WideString& str) const
        {
            return Ame::StringHash{ str };
        }
    };
} // namespace std
