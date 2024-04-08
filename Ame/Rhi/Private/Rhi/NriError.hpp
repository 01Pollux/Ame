#pragma once

#include <Core/Ame.hpp>
#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    template<typename... ArgsTy>
    void ThrowIfFailed(
        nri::Result                         Result,
        const std::format_string<ArgsTy...> Message,
        ArgsTy&&... Args)
    {
#ifndef AME_DIST
        Log::Rhi().Assert(Result == nri::Result::SUCCESS, std::move(Message), std::forward<ArgsTy>(Args)...);
#else
        if (Result != nri::Result::SUCCESS)
        {
            Log::Rhi().Error(std::move(Message), std::forward<ArgsTy>(Args)...);
        }
#endif
    }
} // namespace Ame::Rhi