#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>
#include <Log/Core.hpp>

namespace Ame::Events
{
    struct LogEvent
    {
        StringU8View  Message;
        Log::LogLevel Level;
    };
} // namespace Ame::Events

AME_SIGNAL_DECL(const Ame::Log::Logger, OnLog, const Ame::Events::LogEvent& /*Data*/);
