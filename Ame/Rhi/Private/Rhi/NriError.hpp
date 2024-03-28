#pragma once

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
#ifndef AME_DIST
    template<typename... ArgsTy>
    void ThrowIfFailed(
        nri::Result Result,
        const char* Message,
        ArgsTy&&... Args)
    {
        if (Result != nri::Result::SUCCESS)
        {
            Log::Rhi().Error("Error: %s", Message, std::forward<ArgsTy>(Args)...);
        }
    }
#else
    void ThrowIfFailed(
        nri::Result Result,
        ...)
    {
    }
#endif
} // namespace Ame::Rhi