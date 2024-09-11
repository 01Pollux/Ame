#pragma once

#include <Log/Core.hpp>
#include <Core/Interface.hpp>

namespace spdlog::sinks
{
    class sink;
} // namespace spdlog::sinks

namespace Ame::Log
{
    class AME_ENGINE_API ILoggerStream
    {
    public:
        ILoggerStream() = default;

        ILoggerStream(const ILoggerStream&)            = delete;
        ILoggerStream& operator=(const ILoggerStream&) = delete;

        ILoggerStream(ILoggerStream&&) noexcept            = delete;
        ILoggerStream& operator=(ILoggerStream&&) noexcept = delete;

        virtual ~ILoggerStream();

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
