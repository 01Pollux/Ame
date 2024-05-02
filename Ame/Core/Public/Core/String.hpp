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
    static Ty Empty = {};

    /// <summary>
    /// Transform string to another type
    /// </summary>
    template<typename ToTy, typename FromTy>
    [[nodiscard]] constexpr ToTy To(
        const FromTy& Str) noexcept
    {
        // same type
        if constexpr (std::is_same_v<ToTy, FromTy>)
            return Str;
        // from const _char* to _string_view
        else if constexpr (std::is_pointer_v<FromTy>)
            return To<ToTy>(std::basic_string_view<std::remove_pointer_t<FromTy>>(Str));
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
    [[nodiscard]] static String ToLower(
        const StringView& Str) noexcept
    {
        String LowStr;
        LowStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(LowStr), [](Char c)
                               { return static_cast<Char>(std::tolower(c)); });
        return LowStr;
    }

    /// <summary>
    /// Convert string to lower case
    /// </summary>
    [[nodiscard]] static WideString ToLower(
        const WideStringView& Str) noexcept
    {
        WideString LowStr;
        LowStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(LowStr), [](WideChar c)
                               { return static_cast<WideChar>(std::tolower(c)); });
        return LowStr;
    }

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] static String ToUpper(
        const StringView& Str) noexcept
    {
        String UpStr;
        UpStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(UpStr), [](Char c)
                               { return static_cast<Char>(std::toupper(c)); });
        return UpStr;
    }

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] static WideString ToUpper(
        const WideStringView& Str) noexcept
    {
        WideString UpStr;
        UpStr.reserve(Str.size());
        std::ranges::transform(Str, std::back_inserter(UpStr), [](WideChar c)
                               { return static_cast<WideChar>(std::toupper(c)); });
        return UpStr;
    }

    //

    /// <summary>
    /// Replace occurence of a token in a string
    /// </summary>
    static bool Replace(
        String&    Str,
        StringView Token,
        StringView Value) noexcept
    {
        size_t It = Str.find(Token.data(), 0, Token.size());
        if (It != Str.npos)
        {
            Str.replace(It, Token.size(), Value);
            return true;
        }
        return false;
    }

    /// <summary>
    /// Replace occurence of a token in a string
    /// </summary>
    static bool Replace(
        WideString&    Str,
        WideStringView Token,
        WideStringView Value) noexcept
    {
        size_t It = Str.find(Token.data(), 0, Token.size());
        if (It != Str.npos)
        {
            Str.replace(It, Token.size(), Value);
            return true;
        }
        return false;
    }

    //

    /// <summary>
    /// Replace all occurences of a token in a string
    /// </summary>
    static size_t ReplaceAll(
        String&    Str,
        StringView Token,
        StringView Value) noexcept
    {
        size_t Count = 0;
        while (true)
        {
            size_t It = Str.find(Token.data(), 0, Token.size());
            if (It != Str.npos)
            {
                Str.replace(It, Token.size(), Value);
                Count++;
            }
            else
            {
                break;
            }
        }
        return Count;
    }

    /// <summary>
    /// Replace all occurences of a token in a string
    /// </summary>
    static size_t ReplaceAll(
        WideString&    Str,
        WideStringView Token,
        WideStringView Value) noexcept
    {
        size_t Count = 0;
        while (true)
        {
            size_t It = Str.find(Token.data(), 0, Token.size());
            if (It != Str.npos)
            {
                Str.replace(It, Token.size(), Value);
                Count++;
            }
            else
            {
                break;
            }
        }
        return Count;
    }

    //

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
} // namespace Ame::Strings

namespace Ame
{
    struct StringHash
    {
    public:
        template<Concepts::StringType Ty>
        constexpr StringHash(
            const Ty& Str) :
            StringHash(Strings::Hash(Str))
        {
        }

        constexpr StringHash(
            size_t Hash) :
            m_Hash(Hash)
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
            const char* Str,
            size_t      Size)
        {
            return Strings::Hash(std::string_view(Str, Size));
        }

        [[nodiscard]] constexpr StringHash operator""_hash(
            const wchar_t* Str,
            size_t         Size)
        {
            return Strings::Hash(std::wstring_view(Str, Size));
        }
    } // namespace Literals
} // namespace Ame

namespace std
{
    template<>
    struct hash<Ame::StringView>
    {
        size_t operator()(const Ame::StringView& Str) const
        {
            return Ame::StringHash{ Str };
        }
    };

    template<>
    struct hash<Ame::WideStringView>
    {
        size_t operator()(const Ame::WideStringView& Str) const
        {
            return Ame::StringHash{ Str };
        }
    };

    template<>
    struct hash<Ame::String>
    {
        size_t operator()(const Ame::String& Str) const
        {
            return Ame::StringHash{ Str };
        }
    };

    template<>
    struct hash<Ame::WideString>
    {
        size_t operator()(const Ame::WideString& Str) const
        {
            return Ame::StringHash{ Str };
        }
    };
} // namespace std
