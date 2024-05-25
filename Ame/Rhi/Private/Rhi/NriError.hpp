#pragma once

#include <Core/Ame.hpp>
#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    template<typename... ArgsTy>
    void ThrowIfFailed(
        nri::Result                         result,
        const std::format_string<ArgsTy...> message,
        ArgsTy&&... args)
    {
#ifndef AME_DIST
        Log::Rhi().Assert(result == nri::Result::SUCCESS, std::move(message), std::forward<ArgsTy>(args)...);
#else
        if (result != nri::Result::SUCCESS)
        {
            Log::Rhi().Error(std::move(message), std::forward<ArgsTy>(args)...);
        }
#endif
    }
} // namespace Ame::Rhi