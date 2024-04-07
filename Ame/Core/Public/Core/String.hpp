#pragma once

#include <Core/Ame.hpp>
#include <EASTL/string.h>
#include <EASTL/string_view.h>
#include <format>
#include <algorithm>

#if !EASTL_DLL
inline void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
    return new uint8_t[size];
}
#endif

namespace Ame
{
    template<typename CharTy>
    class BasicStringView : public eastl::basic_string_view<CharTy>
    {
    public:
        using char_type            = CharTy;
        using std_string_view_type = std::basic_string_view<CharTy>;
        using eastl::basic_string_view<CharTy>::basic_string_view;

    public:
        using eastl::basic_string_view<CharTy>::data;
        using eastl::basic_string_view<CharTy>::size;

        [[nodiscard]] constexpr std_string_view_type std_view() const noexcept
        {
            return { data(), size() };
        }

        [[nodiscard]] constexpr operator std_string_view_type() const noexcept
        {
            return std_view();
        }
    };

    template<typename CharTy>
    class BasicString : public eastl::basic_string<CharTy>
    {
    public:
        using char_type            = CharTy;
        using string_view_type     = BasicStringView<CharTy>;
        using std_string_view_type = std::basic_string_view<CharTy>;

        using eastl::basic_string<CharTy>::basic_string;

        template<typename... ArgsTy>
        [[nodiscard]] static BasicString<CharTy> formatted(
            const std_string_view_type Format,
            ArgsTy&&... Args);

    public:
        using eastl::basic_string<CharTy>::data;
        using eastl::basic_string<CharTy>::size;

        [[nodiscard]] constexpr string_view_type view() const noexcept
        {
            return { data(), size() };
        }

        [[nodiscard]] constexpr std_string_view_type std_view() const noexcept
        {
            return { data(), size() };
        }

        [[nodiscard]] constexpr operator std_string_view_type() const noexcept
        {
            return std_view();
        }

        [[nodiscard]] constexpr operator string_view_type() const noexcept
        {
            return view();
        }
    };

    using CharU8       = char;
    using StringU8View = BasicStringView<CharU8>;
    using StringU8     = BasicString<CharU8>;

    using Char       = wchar_t;
    using StringView = BasicStringView<Char>;
    using String     = BasicString<Char>;
} // namespace Ame

namespace Ame
{
    namespace Concepts
    {
        /// <summary>
        /// String type concept
        /// </summary>
        template<typename Ty>
        concept StringType = std::is_same_v<Ty, StringU8> || std::is_same_v<Ty, String> ||
                             std::is_same_v<Ty, StringU8View> || std::is_same_v<Ty, StringView> ||
                             std::is_same_v<Ty, std::string> || std::is_same_v<Ty, std::wstring> ||
                             std::is_same_v<Ty, std::string_view> || std::is_same_v<Ty, std::wstring_view> ||
                             std::is_same_v<std::remove_cv_t<std::remove_pointer_t<std::decay_t<Ty>>>, char> ||
                             std::is_same_v<std::remove_cv_t<std::remove_pointer_t<std::decay_t<Ty>>>, wchar_t>;
    } // namespace Concepts
} // namespace Ame

namespace Ame::Strings
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
    [[nodiscard]] static StringU8 ToLower(
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
    [[nodiscard]] static String ToUpper(
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
    [[nodiscard]] static StringU8 ToUpper(
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
    /// Replace occurence of a token in a string
    /// </summary>
    static bool Replace(
        StringU8&       Str,
        StringU8View    Token,
        const StringU8& Value) noexcept
    {
        size_t It = Str.find(Token.data(), 0, Token.size());
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
    static bool ReplaceAll(
        StringU8&       Str,
        StringU8View    Token,
        const StringU8& Value) noexcept
    {
        bool Replaced = false;
        while (true)
        {
            size_t It = Str.find(Token.data(), 0, Token.size());
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
    struct hash<Ame::StringU8View>
    {
        size_t operator()(const Ame::StringU8View& Str) const
        {
            return Ame::StringHash{ Str };
        }
    };

    template<>
    struct hash<Ame::StringView>
    {
        size_t operator()(const Ame::StringView& Str) const
        {
            return Ame::StringHash{ Str };
        }
    };

    template<>
    struct hash<Ame::StringU8>
    {
        size_t operator()(const Ame::StringU8& Str) const
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
} // namespace std

namespace std
{
    template<typename CharTy>
    struct formatter<Ame::BasicStringView<CharTy>, CharTy>
    {
        template<typename ParseContext>
        constexpr auto parse(ParseContext& Ctx)
        {
            auto It = Ctx.begin();
            if (It != Ctx.end() && *It != '}')
                throw std::format_error("invalid format");
            return It;
        }

        template<typename FormatContext>
        auto format(const Ame::BasicStringView<CharTy> Str, FormatContext& Ctx) const
        {
            return std::ranges::copy(Str.std_view(), Ctx.out()).out;
        }
    };

    template<typename CharTy>
    struct formatter<Ame::BasicString<CharTy>, CharTy> : formatter<Ame::BasicStringView<CharTy>, CharTy>
    {
        using base_class = formatter<Ame::BasicStringView<CharTy>, CharTy>;

        template<typename ParseContext>
        constexpr auto parse(ParseContext& Ctx)
        {
            return base_class::parse(Ctx);
        }

        template<typename FormatContext>
        auto format(const Ame::BasicString<CharTy>& Str, FormatContext& Ctx) const
        {
            return base_class::format(Str, Ctx);
        }
    };
} // namespace std

namespace Ame
{
    template<typename CharTy>
    template<typename... ArgsTy>
    [[nodiscard]] inline BasicString<CharTy> BasicString<CharTy>::formatted(
        const std_string_view_type Format,
        ArgsTy&&... Args)
    {
        BasicString<CharTy> Str;
        Str.reserve(Format.size());
        std::vformat_to(std::back_inserter(Str), std::move(Format), std::make_format_args(std::forward<ArgsTy>(Args)...));
        return Str;
    }
} // namespace Ame