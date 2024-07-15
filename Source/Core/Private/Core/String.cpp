#include <ranges>
#include <Core/String.hpp>

namespace Ame
{
    String ToLower(
        StringView str) noexcept
    {
        return str |
               std::views::transform([](Char c)
                                     { return static_cast<Char>(std::tolower(c)); }) |
               std::ranges::to<String>();
    }

    WideString ToLower(
        WideStringView str) noexcept
    {
        return str |
               std::views::transform([](WideChar c)
                                     { return static_cast<WideChar>(std::tolower(c)); }) |
               std::ranges::to<WideString>();
    }

    //

    String ToUpper(
        StringView str) noexcept
    {
        return str |
               std::views::transform([](Char c)
                                     { return static_cast<Char>(std::toupper(c)); }) |
               std::ranges::to<String>();
    }

    WideString ToUpper(
        WideStringView str) noexcept
    {
        return str |
               std::views::transform([](WideChar c)
                                     { return static_cast<WideChar>(std::toupper(c)); }) |
               std::ranges::to<WideString>();
    }

    //

    bool Replace(
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

    bool Replace(
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

    size_t ReplaceAll(
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

    //

    size_t ReplaceAll(
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
} // namespace Ame