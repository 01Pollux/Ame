#include <Log/Stream.hpp>
#include <Log/Utils.hpp>

namespace Ame::Log
{
    void ILoggerStream::SetPattern(String pattern)
    {
        m_Pattern = std::move(pattern);
        m_Sink->set_pattern(m_Pattern);
    }

    void ILoggerStream::SetLevel(LogLevel level)
    {
        m_Sink->set_level(GetSpdlogLevel(level));
    }

    const String& ILoggerStream::GetPattern() const noexcept
    {
        return m_Pattern;
    }

    LogLevel ILoggerStream::GetLevel() const
    {
        return GetLogLevel(m_Sink->level());
    }

    const SharedPtr<spdlog::sinks::sink>& ILoggerStream::GetSink() const noexcept
    {
        return m_Sink;
    }
} // namespace Ame::Log
