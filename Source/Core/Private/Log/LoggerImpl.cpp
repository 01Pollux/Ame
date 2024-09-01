#include <filesystem>
#include <ranges>
#include <map>

#include <Log/LoggerImpl.hpp>
#include <Log/Stream.hpp>
#include <Log/Utils.hpp>

namespace Ame::Log
{
    Ptr<ILogger> CreateLogger(String name)
    {
        return AmeCreate(LoggerImpl, std::move(name));
    }

    LoggerImpl::LoggerImpl(IReferenceCounters* counters, String loggerName) :
        Base(counters), m_Logger(std::move(loggerName))
    {
#if defined AME_DEBUG
        SetLevel(LogLevel::Trace);
#elif defined AME_RELEASE
        SetLevel(LogLevel::Info);
#elif defined AME_DIST
        SetLevel(LogLevel::Error);
#endif
        m_Logger.flush_on(spdlog::level::err);
    }

    LoggerImpl::~LoggerImpl()
    {
        spdlog::drop(m_Logger.name());
    }

    void LoggerImpl::WriteMessage(const LoggerInfo& info)
    {
#ifndef AME_DISABLE_LOGGING
        if (!CanLog(info.Level))
        {
            return;
        }
        m_Logger.log(GetSpdlogLevel(info.Level), info.Message);

        if (info.Level >= LogLevel::Error)
        {
            m_Logger.dump_backtrace();
        }
#endif
    }

    void LoggerImpl::AddStream(ILoggerStream* stream)
    {
        m_Logger.sinks().push_back(stream->GetSink());
    }

    void LoggerImpl::RemoveStream(ILoggerStream* stream)
    {
        std::erase(m_Logger.sinks(), stream->GetSink());
    }

    LogLevel LoggerImpl::GetLevel() const noexcept
    {
        return GetLogLevel(m_Logger.level());
    }

    void LoggerImpl::SetLevel(LogLevel level) noexcept
    {
        m_Logger.set_level(GetSpdlogLevel(level));
    }
} // namespace Ame::Log
