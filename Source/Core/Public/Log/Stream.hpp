#pragma once

#include <Log/Core.hpp>
#include <Core/Interface.hpp>

namespace spdlog::sinks
{
    class sink;
} // namespace spdlog::sinks

namespace Ame::Log
{
    class ILoggerStream
    {
    public:
        virtual ~ILoggerStream() = default;

    public:
        void SetPattern(String pattern);
        void SetLevel(LogLevel level);

        [[nodiscard]] const String& GetPattern() const noexcept;
        [[nodiscard]] LogLevel      GetLevel() const;

        [[nodiscard]] const SharedPtr<spdlog::sinks::sink>& GetSink() const noexcept;

    protected:
        String                         m_Pattern;
        SharedPtr<spdlog::sinks::sink> m_Sink;
    };
} // namespace Ame::Log
