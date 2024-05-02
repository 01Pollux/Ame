#include <Log/Logger.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/msvc_sink.h>

#include <filesystem>
#include <ranges>
#include <map>

namespace Ame::Log
{
    static std::map<String, UPtr<Logger>> s_Loggers;

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
        StringView TagName,
        StringView FileName,
        SinkList   Sinks) :
        m_Name(TagName)
    {
        m_Logger = std::make_unique<spdlog::logger>(Strings::To<std::string>(m_Name), Sinks.begin(), Sinks.end());
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
        const String& TagName,
        StringView    FileName,
        SinkList      Sinks)
    {
        if (s_Loggers.contains(TagName))
        {
            return;
        }

        s_Loggers.emplace(TagName, UPtr<Logger>(new Logger(TagName, FileName, std::move(Sinks))));
    }

    void Logger::Register(
        const String& TagName,
        StringView    FileName)
    {
        if (s_Loggers.contains(TagName))
        {
            return;
        }

        EnsureLogsDirectory();

        SinkList Sinks{
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::format("Logs/{}", Strings::To<std::string_view>(FileName))),
#ifndef AME_DIST
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
#ifdef AME_PLATFORM_WINDOWS
            std::make_shared<spdlog::sinks::msvc_sink_mt>()
#endif
#endif
        };

        // first sink is the file sink
        Sinks[0]->set_pattern("[%c] [%l] %n :: %v.");

#ifndef AME_DIST
        for (auto& Sink : Sinks | std::views::drop(1))
        {
            Sink->set_pattern("%^[%T] [%l] %n :: %v%$.");
        }
#endif

        s_Loggers.emplace(TagName, UPtr<Logger>(new Logger(TagName, FileName, std::move(Sinks))));
    }

    void Logger::RegisterNull(
        const String& TagName)
    {
        s_Loggers.emplace(TagName, UPtr<Logger>(new Logger));
    }

    void Logger::Unregister(
        const String& TagName)
    {
        spdlog::drop(Strings::To<std::string>(TagName));
        s_Loggers.erase(TagName);
    }

    //

    Logger& Logger::GetLogger(
        const String& Name)
    {
        static Logger s_NullLogger;
        auto          Iter = s_Loggers.find(Name);
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
        LogLevel Level) const noexcept
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
        return CurLevel >= Level;
    }

    void Logger::LogMessage(
        LogLevel   Level,
        StringView Message) const
    {
        if (!m_Logger) [[unlikely]]
        {
            return;
        }

        switch (Level)
        {
        case LogLevel::Trace:
            m_Logger->trace(Message);
            break;
        case LogLevel::Debug:
            m_Logger->debug(Message);
            break;
        case LogLevel::Info:
            m_Logger->info(Message);
            break;
        case LogLevel::Warning:
            m_Logger->warn(Message);
            break;
        case LogLevel::Error:
            m_Logger->error(Message);
            break;
        case LogLevel::Fatal:
            m_Logger->critical(Message);
            break;
        }

        StaticOnLog().Broadcast(*this, { Message, Level });
    }

    void Logger::SetLevel(
        LogLevel Level)
    {
        if (!m_Logger) [[unlikely]]
        {
            return;
        }

        switch (Level)
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
} // namespace Ame::Log
