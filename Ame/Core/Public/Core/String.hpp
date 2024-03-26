#pragma once

#include <Core/Ame.hpp>
#include <string>
#include <format>
#include <algorithm>

#define STR(x)   L##x
#define STRU8(x) StringU8(x)

namespace Ame
{
    using CharU8       = char;
    using StringU8     = std::string;
    using StringU8View = std::string_view;

    using Char       = wchar_t;
    using String     = std::wstring;
    using StringView = std::wstring_view;

    namespace Concepts
    {
        /// <summary>
        /// String type concept
        /// </summary>
        template<typename Ty>
        concept StringType = std::is_same_v<Ty, StringU8> || std::is_same_v<Ty, String> ||
                             std::is_same_v<Ty, StringU8View> || std::is_same_v<Ty, StringView> ||
                             std::is_same_v<std::remove_cv_t<std::remove_pointer_t<std::decay_t<Ty>>>, char> ||
                             std::is_same_v<std::remove_cv_t<std::remove_pointer_t<std::decay_t<Ty>>>, wchar_t>;
    } // namespace Concepts

    template<typename Ty>
    struct StringHash;

    template<>
    struct StringHash<StringU8View>
    {
        using hash_type      = std::hash<StringU8View>;
        using is_transparent = void;

        size_t operator()(const char* Str) const
        {
            return std::hash<StringU8View>{}(Str);
        }

        size_t operator()(StringU8View Str) const
        {
            return std::hash<StringU8View>{}(Str);
        }

        size_t operator()(StringU8 const& Str) const
        {
            return std::hash<StringU8>{}(Str);
        }
    };

    template<>
    struct StringHash<StringView>
    {
        using hash_type      = std::hash<StringView>;
        using is_transparent = void;

        size_t operator()(const wchar_t* Str) const
        {
            return std::hash<StringView>{}(Str);
        }

        size_t operator()(StringView Str) const
        {
            return std::hash<StringView>{}(Str);
        }

        size_t operator()(const String& Str) const
        {
            return std::hash<String>{}(Str);
        }
    };
} // namespace Ame

namespace Ame::StringUtils
{
    /// <summary>
    /// Empty string
    /// </summary>
    template<typename Ty>
        requires std::is_same_v<Ty, StringU8> || std::is_same_v<Ty, String>
    static Ty Empty = {};

    /// <summary>
    /// Transform string to another type
    /// </summary>
    template<typename ToTy, typename FromTy>
    [[nodiscard]] constexpr ToTy Transform(
        const FromTy& Str) noexcept
    {
        // same type
        if constexpr (std::is_same_v<ToTy, FromTy>)
            return Str;
        // from const _char* to _string_view
        else if constexpr (std::is_pointer_v<FromTy>)
            return Transform<ToTy>(std::basic_string_view<std::remove_pointer_t<FromTy>>(Str));
        else
        {
            if (Str.empty())
                return {};
            else
            {
                // from bigger type to smaller type, eg: wstring to string, u32string to u8string, etc...
                if constexpr (sizeof(typename FromTy::value_type) > sizeof(typename ToTy::value_type))
                {
                    ToTy buf(Str.size(), 0);
                    std::transform(std::begin(Str), std::end(Str), std::begin(buf), [](const typename FromTy::value_type c)
                                   { return static_cast<typename ToTy::value_type>(c); });
                    return buf;
                }
                // from smaller type to bigger type, eg: string to wstring, etc...
                else
                    return { std::begin(Str), std::end(Str) };
            }
        }
    }

    /// <summary>
    /// Transform string to another type
    /// </summary>
    template<typename ToTy, typename FromTy, size_t Size>
    [[nodiscard]] constexpr ToTy Transform(
        const FromTy (&Str)[Size]) noexcept
    {
        if constexpr (std::is_same_v<typename ToTy::value_type, FromTy>)
            return ToTy{ Str };
        else if constexpr (Size < 1)
            return ToTy{};
        else
        {
            ToTy ToStr{};
            ToStr.reserve(Size);
            std::transform(std::begin(Str), std::end(Str), std::back_inserter(ToStr), [](FromTy c)
                           { return static_cast<typename ToTy::value_type>(c); });
            return ToStr;
        }
    }

    //

    /// <summary>
    /// Convert string to lower case
    /// </summary>
    [[nodiscard]] constexpr String ToLower(
        const String& Str) noexcept
    {
        String LowStr;
        LowStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(LowStr), [](wchar_t c)
                               { return static_cast<wchar_t>(std::tolower(c)); });
        return LowStr;
    }

    /// <summary>
    /// Convert string to lower case
    /// </summary>
    [[nodiscard]] constexpr StringU8 ToLower(
        const StringU8& Str) noexcept
    {
        StringU8 LowStr;
        LowStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(LowStr), [](char c)
                               { return static_cast<char>(std::tolower(c)); });
        return LowStr;
    }

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] constexpr String ToUpper(
        const String& Str) noexcept
    {
        String UpStr;
        UpStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(UpStr), [](wchar_t c)
                               { return static_cast<wchar_t>(std::toupper(c)); });
        return UpStr;
    }

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] constexpr StringU8 ToUpper(
        const StringU8& Str) noexcept
    {
        StringU8 UpStr;
        UpStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(UpStr), [](char c)
                               { return static_cast<char>(std::toupper(c)); });
        return UpStr;
    }

    //

    /// <summary>
    /// Format string
    /// </summary>
    template<typename... ArgsTy>
    [[nodiscard]] constexpr String Format(
        const StringView FormatStr,
        ArgsTy&&... Args)
    {
        return std::vformat(FormatStr, std::make_wformat_args(Args...));
    }

    /// <summary>
    /// Format string
    /// </summary>
    template<typename... ArgsTy>
    [[nodiscard]] constexpr StringU8 Format(
        const StringU8View FormatStr,
        ArgsTy&&... Args)
    {
        return std::vformat(FormatStr, std::make_format_args(Args...));
    }

    //

    /// <summary>
    /// Replace occurence of a token in a string
    /// </summary>
    constexpr bool Replace(
        StringU8&       Str,
        StringU8View    Token,
        const StringU8& Value) noexcept
    {
        size_t It = Str.find(Token);
        if (It != std::string::npos)
        {
            Str.replace(It, Token.size(), Value);
            return true;
        }
        return false;
    }

    /// <summary>
    /// Replace all occurences of a token in a string
    /// </summary>
    constexpr bool ReplaceAll(
        StringU8&       Str,
        StringU8View    Token,
        const StringU8& Value) noexcept
    {
        bool Replaced = false;
        while (true)
        {
            size_t It = Str.find(Token);
            if (It != std::string::npos)
            {
                Str.replace(It, Token.size(), Value);
                Replaced = true;
            }
            else
            {
                break;
            }
        }
        return Replaced;
    }

    /// <summary>
    /// Hash a string
    /// </summary>
    template<Concepts::StringType Ty>
    [[nodiscard]] constexpr size_t Hash(
        const Ty& Str)
    {
        // https://github.com/elanthis/constexpr-hash-demo/blob/master/test.cpp
        // FNV-1a constants

        constexpr size_t Hash_Basis = 14695981039346656037ULL;
        constexpr size_t Hash_Prime = 1099511628211ULL;

        size_t Hash = Hash_Basis;
        for (auto c : std::basic_string_view(Str))
        {
            Hash ^= c;
            Hash *= Hash_Prime;
        }
        return Hash;
    }
} // namespace Ame::StringUtils