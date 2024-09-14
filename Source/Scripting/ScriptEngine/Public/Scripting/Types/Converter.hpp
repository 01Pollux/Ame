#pragma once

namespace Ame::Scripting
{
    template<typename Ty> struct NativeConverter
    {
        static auto Wrap(Ty&& value) noexcept
        {
            return std::forward<Ty>(value);
        }
    };
} // namespace Ame::Scripting
