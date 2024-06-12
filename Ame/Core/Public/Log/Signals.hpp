#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>
#include <Log/Core.hpp>

namespace Ame::Signals::Data
{
    struct LogData
    {
        StringView         Message;
        Ame::Log::LogLevel Level;
    };
} // namespace Ame::Signals::Data

AME_SIGNAL_DECL(OnLog, void(const Data::LogData& logData));
