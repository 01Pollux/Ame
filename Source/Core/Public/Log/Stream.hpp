#pragma once

#include <Log/Core.hpp>
#include <Core/Interface.hpp>
#include <spdlog/sinks/sink.h>

namespace Ame::Log
{
    class ILoggerStream
    {
    public:
        void SetPattern(String pattern) noexcept
        {
            m_Pattern = std::move(pattern);
            m_Sink->set_pattern(m_Pattern);
        }

        [[nodiscard]] const String& GetPattern() const noexcept
        {
            return m_Pattern;
        }

        [[nodiscard]] LogLevel GetLevel() const noexcept;
        void                   SetLevel(LogLevel level) noexcept;

        [[nodiscard]] const SharedPtr<spdlog::sinks::sink>& GetSink() const noexcept
        {
            return m_Sink;
        }

    protected:
        String                         m_Pattern;
        SharedPtr<spdlog::sinks::sink> m_Sink;
    };
} // namespace Ame::Log
