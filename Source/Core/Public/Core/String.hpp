#pragma once

#include <string>
#include <string_view>
#include <format>
#include <ranges>

namespace Ame
{
    using Char       = char;
    using StringView = std::string_view;
    using String     = std::string;

    using WideChar       = wchar_t;
    using WideStringView = std::wstring_view;
    using WideString     = std::wstring;

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

} // namespace Ame

namespace Ame::Strings
{
    /// <summary>
    /// Empty string
    /// </summary>
    template<typename Ty>
        requires std::is_same_v<Ty, String> || std::is_same_v<Ty, WideString>
    static const Ty c_Empty = {};

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
                return str |
                       std::views::transform([](const typename FromTy::value_type c)
                                             { return static_cast<typename ToTy::value_type>(c); }) |
                       std::ranges::to<ToTy>();
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
            return To<ToTy>(std::basic_string_view<FromTy>(str, Size));
        }
    }

    //

    /// <summary>
    /// Convert string to lower case
    /// </summary>
    [[nodiscard]] String ToLower(
        StringView str) noexcept;

    /// <summary>
    /// Convert string to lower case
    /// </summary>
    [[nodiscard]] WideString ToLower(
        WideStringView str) noexcept;

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] String ToUpper(
        StringView str) noexcept;

    /// <summary>
    /// Convert string to upper case
    /// </summary>
    [[nodiscard]] WideString ToUpper(
        WideStringView str) noexcept;

    //

    /// <summary>
    /// Replace occurence of a token in a string
    /// </summary>
    bool Replace(
        String&    str,
        StringView token,
        StringView value) noexcept;

    /// <summary>
    /// Replace occurence of a token in a string
    /// </summary>
    bool Replace(
        WideString&    str,
        WideStringView token,
        WideStringView value) noexcept;

    //

    /// <summary>
    /// Replace all occurences of a token in a string
    /// </summary>
    size_t ReplaceAll(
        String&    str,
        StringView token,
        StringView value) noexcept;

    /// <summary>
    /// Replace all occurences of a token in a string
    /// </summary>
    size_t ReplaceAll(
        WideString&    str,
        WideStringView token,
        WideStringView value) noexcept;

    //

    /// <summary>
    /// Hash a string
    /// </summary>
    template<StringType Ty>
    [[nodiscard]] constexpr size_t Hash(
        const Ty& str)
    {
        // https://github.com/elanthis/constexpr-hash-demo/blob/master/test.cpp
        // FNV-1a constants

        constexpr size_t c_HashBasis = 14695981039346656037ULL;
        constexpr size_t c_HashPrime = 1099511628211ULL;

        size_t hash = c_HashBasis;
        for (auto it = std::begin(str); it != std::end(str); ++it)
        {
            hash ^= static_cast<size_t>(*it);
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
        template<StringType Ty>
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
} // namespace std
