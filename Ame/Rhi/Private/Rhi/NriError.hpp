#pragma once

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    template<typename... ArgsTy>
    void ThrowIfFailed(
        nri::Result Result,
        const char* Message,
        ArgsTy&&... Args)
    {
#ifndef AME_DIST
        Log::Rhi().Assert(Result == nri::Result::SUCCESS, "Error: {}", Message, std::forward<ArgsTy>(Args)...);
#else
        if (Result != nri::Result::SUCCESS)
        {
            Log::Rhi().Error("Error: {}", Message, std::forward<ArgsTy>(Args)...);
        }
#endif
    }
} // namespace Ame::Rhi