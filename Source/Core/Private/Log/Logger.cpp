#include <filesystem>
#include <stacktrace>
#include <boost/stacktrace.hpp>
#include <ranges>
#include <map>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/msvc_sink.h>

#include <Log/Logger.hpp>

namespace Ame::Log
{
    static std::map<String, UniquePtr<Logger>> s_Loggers;

    //

    /// <summary>
    /// Ensure that the logs directory exists
    /// </summary>
    static void EnsureLogsDirectory()
    {
        [[maybe_unused]] std::error_code ErrorCode;
        std::filesystem::create_directories("Logs", ErrorCode);
    }

    Logger::Logger(
        StringView tagName,
        SinkList   sinks)
    {
        m_Logger = std::make_unique<spdlog::logger>(Strings::To<std::string>(tagName), sinks.begin(), sinks.end());
        m_Logger->flush_on(spdlog::level::err);

#if defined AME_DEBUG
        SetLevel(LogLevel::Trace);
#elif defined AME_RELEASE
        SetLevel(LogLevel::Info);
#elif defined AME_DIST
        SetLevel(LogLevel::Error);
#endif
    }

    //

    void Logger::Register(
        const String& tagName,
        SinkList      sinks)
    {
        if (s_Loggers.contains(tagName))
        {
            return;
        }

        s_Loggers.emplace(tagName, UniquePtr<Logger>(new Logger(tagName, std::move(sinks))));
    }

    void Logger::Register(
        const String& tagName,
        StringView    fileName)
    {
        if (s_Loggers.contains(tagName))
        {
            return;
        }

        EnsureLogsDirectory();

        SinkList Sinks{
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(String(fileName)),
#ifndef AME_DIST
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
#ifdef AME_PLATFORM_WINDOWS
            std::make_shared<spdlog::sinks::msvc_sink_mt>()
#endif
#endif
        };

        // first sink is the file sink
        Sinks[0]->set_pattern("[%c] [%l] %n :: %v");

#ifndef AME_DIST
        for (auto& Sink : Sinks | std::views::drop(1))
        {
            Sink->set_pattern("%^[%T] [%l] %n :: %v%$");
        }
#endif

        s_Loggers.emplace(tagName, UniquePtr<Logger>(new Logger(tagName, std::move(Sinks))));
    }

    void Logger::RegisterNull(
        const String& tagName)
    {
        s_Loggers.emplace(tagName, UniquePtr<Logger>(new Logger));
    }

    void Logger::Unregister(
        const String& tagName)
    {
        spdlog::drop(Strings::To<std::string>(tagName));
        s_Loggers.erase(tagName);
    }

    //

    Logger& Logger::GetLogger(
        const String& name)
    {
        static Logger s_NullLogger;
        auto          Iter = s_Loggers.find(name);
        if (Iter != s_Loggers.end())
        {
            return *Iter->second;
        }
        return s_NullLogger;
    }

    void Logger::CloseAllLoggers()
    {
        spdlog::drop_all();
        s_Loggers.clear();
    }

    //

    bool Logger::CanLog(
        LogLevel level) const noexcept
    {
        if (!m_Logger) [[unlikely]]
        {
            return false;
        }

        LogLevel CurLevel = GetLevel();
#ifdef AME_DIST
        if (CurLevel < Log::LogLevel::Warning)
        {
            return false;
        }
#endif
        return CurLevel >= level;
    }

    void Logger::LogMessage(
        LogLevel   level,
        StringView message) const
    {
#ifndef AME_DISABLE_LOGGING
        if (!m_Logger) [[unlikely]]
        {
            return;
        }

        auto logStackTrace = [&](spdlog::level::level_enum level)
        {
#ifndef AME_DIST
            auto stack = std::stacktrace::current(1);
            auto it    = stack.begin();
            // Skip logger calls
            for (it; it != stack.end() && it->source_file().contains("Logger"); it++)
            {
            }
            // Print stacktrace
            if (it != stack.end())
            {
                m_Logger->log(level, "Dumping Stacktrace:");
                int index = 0;
                while (it != stack.end())
                {
                    m_Logger->log(level, "{}> {}({}): {}", index++, it->source_file(), it->source_line(), it->description());
                    it++;
                }
            }
#endif
        };

        switch (level)
        {
#ifndef AME_DIST
        case LogLevel::Trace:
            m_Logger->trace(message);
            break;
        case LogLevel::Debug:
            m_Logger->debug(message);
            break;
        case LogLevel::Info:
            m_Logger->info(message);
            break;
#endif
        case LogLevel::Warning:
            m_Logger->warn(message);
            break;
        case LogLevel::Error:
            m_Logger->error(message);
            logStackTrace(spdlog::level::err);
            break;
        case LogLevel::Fatal:
            m_Logger->critical(message);
            logStackTrace(spdlog::level::critical);
            throw std::runtime_error(message.data());
            break;
        default:
        {
            return;
        }
        }
#endif
    }

    void Logger::SetLevel(
        LogLevel level)
    {
        if (!m_Logger) [[unlikely]]
        {
            return;
        }

        switch (level)
        {
        case LogLevel::Disabled:
            m_Logger->set_level(spdlog::level::off);
            break;
        case LogLevel::Trace:
            m_Logger->set_level(spdlog::level::trace);
            break;
        case LogLevel::Debug:
            m_Logger->set_level(spdlog::level::debug);
            break;
        case LogLevel::Info:
            m_Logger->set_level(spdlog::level::info);
            break;
        case LogLevel::Warning:
            m_Logger->set_level(spdlog::level::warn);
            break;
        case LogLevel::Error:
            m_Logger->set_level(spdlog::level::err);
            break;
        case LogLevel::Fatal:
            m_Logger->set_level(spdlog::level::critical);
            break;
        }
    }

    LogLevel Logger::GetLevel() const noexcept
    {
        if (!m_Logger) [[unlikely]]
        {
            return LogLevel::Disabled;
        }

        switch (m_Logger->level())
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

    //

    String FormatException(
        const std::exception& ex)
    {
        String message = ex.what();
#ifndef AME_DIST
        auto stack = boost::stacktrace::stacktrace::from_current_exception();
        if (stack)
        {
            auto it = stack.begin();
            // Skip logger calls
            for (it; it != stack.end() && it->source_file().contains("Logger"); it++)
            {
            }
            if (it != stack.end())
            {
                message += "\nDumping Stacktrace:";
                for (it; it != stack.end(); it++)
                {
                    message += "\n" + String(it->source_file().data()) + ":" + std::to_string(it->source_line());
                }
            }
            return message;
        }
#endif
        return message;
    }
} // namespace Ame::Log
