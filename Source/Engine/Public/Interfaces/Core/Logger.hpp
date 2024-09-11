#pragma once

#include <Log/Logger.hpp>

namespace Ame::Interfaces
{
    // {940017D2-269C-45B6-803B-F3C530151CCA}
    inline constexpr UId IID_Logger = Log::IID_Logger;

    using ILogger = Ame::Log::ILogger;
} // namespace Ame::Interfaces
