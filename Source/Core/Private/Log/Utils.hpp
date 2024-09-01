#pragma once

#include <Log/Core.hpp>
#include <Log/Streams/LogFileEvent.hpp>
#include <spdlog/sinks/sink.h>

namespace Ame::Log
{
    [[nodiscard]] inline spdlog::level::level_enum GetSpdlogLevel(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Trace:
            return spdlog::level::trace;
        case LogLevel::Debug:
            return spdlog::level::debug;
        case LogLevel::Info:
            return spdlog::level::info;
        case LogLevel::Warning:
            return spdlog::level::warn;
        case LogLevel::Error:
            return spdlog::level::err;
        case LogLevel::Fatal:
            return spdlog::level::critical;
        default:
            return spdlog::level::off;
        }
    }

    [[nodiscard]] inline LogLevel GetLogLevel(spdlog::level::level_enum level)
    {
        switch (level)
        {
        case spdlog::level::trace:
            return LogLevel::Trace;
        case spdlog::level::debug:
            return LogLevel::Debug;
        case spdlog::level::info:
            return LogLevel::Info;
        case spdlog::level::warn:
            return LogLevel::Warning;
        case spdlog::level::err:
            return LogLevel::Error;
        case spdlog::level::critical:
            return LogLevel::Fatal;
        default:
            return LogLevel::Disabled;
        }
    }

    [[nodiscard]] inline spdlog::file_event_handlers GetSpdlogFileEventHandlers(LogFileEvent events)
    {
        spdlog::file_event_handlers handlers;
        handlers.before_open  = std::move(events.BeforeOpen);
        handlers.after_open   = std::move(events.AfterOpen);
        handlers.before_close = std::move(events.BeforeClose);
        handlers.after_close  = std::move(events.AfterClose);
        return handlers;
    }
} // namespace Ame::Log
