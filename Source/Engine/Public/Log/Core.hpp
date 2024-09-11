#pragma once

#include <source_location>
#include <Core/String.hpp>
#include <Core/Ame.hpp>

namespace Ame::Log
{
    class ILogger;
    class ILoggerStream;

    enum class LogLevel : unsigned char
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
        Disabled,
    };

    struct LoggerInfo
    {
        StringView Message;
#ifndef AME_DIST
        const std::source_location Location;
#endif
        LogLevel Level;

        LoggerInfo(LogLevel level, StringView message
#ifndef AME_DIST
                   ,
                   std::source_location sourceLoc = std::source_location::current()
#endif
                       ) noexcept :

            Level(level),
            Message(message)
#ifndef AME_DIST
            ,
            Location(sourceLoc)
#endif
        {
        }
    };
} // namespace Ame::Log

#ifndef AME_NO_LOGGERS

#define AME_INTERNAL_LOGGER_INST Ame::Log::ILogger::Get()
#define LOG_MESSAGE_IMPL(logger, type, message)                                                                        \
    if (logger) [[likely]]                                                                                             \
    {                                                                                                                  \
        if (logger->CanLog(Ame::Log::LogLevel::type))                                                                  \
        {                                                                                                              \
            logger->WriteMessage({ Ame::Log::LogLevel::type, message });                                               \
        }                                                                                                              \
    }

//

#ifndef AME_DIST

#define AME_LOG_TRACE_EX(logger, ...) LOG_MESSAGE_IMPL(logger, Trace, __VA_ARGS__)
#define AME_LOG_TRACE(...)            AME_LOG_TRACE_EX(AME_INTERNAL_LOGGER_INST, __VA_ARGS__)

#define AME_LOG_DEBUG_EX(logger, ...) LOG_MESSAGE_IMPL(logger, Debug, __VA_ARGS__)
#define AME_LOG_DEBUG(...)            AME_LOG_DEBUG_EX(AME_INTERNAL_LOGGER_INST, __VA_ARGS__)

#define AME_LOG_ASSERT_EX(logger, cond, ...)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            LOG_MESSAGE_IMPL(logger, Error, std::format("Assertion failed: '{}'", #cond));                             \
            LOG_MESSAGE_IMPL(logger, Error, __VA_ARGS__);                                                              \
            Ame::DebugBreak();                                                                                         \
        }                                                                                                              \
    } while (false)
#define AME_LOG_ASSERT(cond, ...) AME_LOG_ASSERT_EX(AME_INTERNAL_LOGGER_INST, cond, __VA_ARGS__)

#define AME_LOG_VALIDATE_EX(logger, cond, ...)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            LOG_MESSAGE_IMPL(logger, Error, std::format("Validation : '{}'", #cond));                                  \
            LOG_MESSAGE_IMPL(logger, Fatal, __VA_ARGS__);                                                              \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (false)

#else

#define AME_LOG_TRACE_EX(...)
#define AME_LOG_TRACE(...)

#define AME_LOG_DEBUG_EX(...)
#define AME_LOG_DEBUG(...)

#define AME_LOG_ASSERT_EX(cond, ...) ((void)(cond))
#define AME_LOG_ASSERT(cond, ...)    ((void)(cond))

#define AME_LOG_VALIDATE_EX(logger, cond, ...)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            LOG_MESSAGE_IMPL(logger, Fatal, __VA_ARGS__);                                                              \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (false)

#endif // AME_DIST

#define AME_LOG_VALIDATE(cond, ...) AME_LOG_VALIDATE_EX(AME_INTERNAL_LOGGER_INST, cond, __VA_ARGS__)

#define AME_LOG_INFO_EX(logger, ...) LOG_MESSAGE_IMPL(logger, Info, __VA_ARGS__)
#define AME_LOG_INFO(...)            AME_LOG_INFO_EX(AME_INTERNAL_LOGGER_INST, __VA_ARGS__)

#define AME_LOG_WARNING_EX(logger, ...) LOG_MESSAGE_IMPL(logger, Warning, __VA_ARGS__)
#define AME_LOG_WARNING(...)            AME_LOG_WARNING_EX(AME_INTERNAL_LOGGER_INST, __VA_ARGS__)

#define AME_LOG_ERROR_EX(logger, ...) LOG_MESSAGE_IMPL(logger, Error, __VA_ARGS__)
#define AME_LOG_ERROR(...)            AME_LOG_ERROR_EX(AME_INTERNAL_LOGGER_INST, __VA_ARGS__)

#define AME_LOG_FATAL_EX(logger, ...) LOG_MESSAGE_IMPL(logger, Fatal, __VA_ARGS__)
#define AME_LOG_FATAL(...)            AME_LOG_FATAL_EX(AME_INTERNAL_LOGGER_INST, __VA_ARGS__)

#else // AME_NO_LOGGERS

#define AME_INTERNAL_LOGGER_INST
#define LOG_MESSAGE_IMPL(...)

#define AME_LOG_TRACE_EX(...)
#define AME_LOG_TRACE(...)
#define AME_LOG_DEBUG_EX(...)
#define AME_LOG_DEBUG(...)
#define AME_LOG_ASSERT_EX(cond, ...)
#define AME_LOG_ASSERT(cond, ...)
#define AME_LOG_VALIDATE_EX(...)
#define AME_LOG_VALIDATE(...)
#define AME_LOG_MESSAGE_EX(...)
#define AME_LOG_MESSAGE(...)
#define AME_LOG_ERROR_EX(...)
#define AME_LOG_ERROR(...)
#define AME_LOG_FATAL_EX(...)
#define AME_LOG_FATAL(...)
#endif // AME_NO_LOGGERS