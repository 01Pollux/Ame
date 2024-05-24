#pragma once

#include <Core/Ame.hpp>
#include <Object/Signal.hpp>
#include <Log/Core.hpp>

namespace Ame::Signals::Data
{
    struct Log
    {
        StringView         Message;
        Ame::Log::LogLevel Level;
    };
} // namespace Ame::Signals::Data

AME_SIGNAL_INSTANCE_DECL(const Ame::Log::Logger, OnLog, const Data::Log& /*logData*/);
