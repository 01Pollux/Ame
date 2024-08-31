#pragma once

#include <vector>

#include <Core/Ame.hpp>
#include <Core/String.hpp>
#include <Log/Core.hpp>

namespace spdlog
{
    class logger;
    namespace sinks
    {
        class sink;
    } // namespace sinks
} // namespace spdlog

#define AME_LOG_TYPE(Type)                                                                                             \
    template<typename... ArgsTy> void Type(const std::format_string<ArgsTy...> message, ArgsTy&&... args) const        \
    {                                                                                                                  \
        Log(LogLevel::Type, std::move(message), std::forward<ArgsTy>(args)...);                                        \
    }                                                                                                                  \
    void Type(StringView message)                                                                                      \
    {                                                                                                                  \
        LogMessage(LogLevel::Type, message);                                                                           \
    }

#ifndef AME_DIST
#define AME_LOG_ASSERT(Logger, Condition, ...) Logger.Assert(Condition, __VA_ARGS__)
#else
#define AME_LOG_ASSERT(Logger, Condition, ...) (static_cast<void>(Condition))
#endif

namespace Ame::Log
{
    class Logger final
    {
    public:
        using Sink     = spdlog::sinks::sink;
        using SinkList = std::vector<SharedPtr<Sink>>;

    private:
        Logger()
        {
        }

        Logger(StringView tagName, SinkList sinks);

    public:
        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(const String& tagName, SinkList sinks);

        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(const String& tagName, StringView fileName);

        /// <summary>
        /// Register a null logger
        /// </summary>
        static void RegisterNull(const String& tagName);

        /// <summary>
        /// Unregister a logger
        /// </summary>
        static void Unregister(const String& tagName);

        /// <summary>
        /// Get global logger
        /// </summary>
        [[nodiscard]] static Logger& GetLogger(const String& name);

        /// <summary>
        /// Close all loggers
        /// </summary>
        static void CloseAllLoggers();

    public:
#ifndef AME_DISABLE_LOGGING
        /// <summary>
        /// Test if a log level can be logged
        /// </summary>
        [[nodiscard]] bool CanLog(LogLevel level) const noexcept;
#else
        [[nodiscard]] bool CanLog(LogLevel) const noexcept
        {
            return false;
        }
#endif

    public:
        /// <summary>
        /// Log a message
        /// </summary>
        void LogMessage(LogLevel level, StringView message) const;

        /// <summary>
        /// Log a message
        /// </summary>
        template<typename... ArgsTy>
        void Log(LogLevel level, const std::format_string<ArgsTy...> message, ArgsTy&&... args) const
        {
#ifndef AME_DISABLE_LOGGING
            if constexpr (sizeof...(ArgsTy) == 0)
            {
                LogMessage(level, message.get());
            }
            else
            {
                LogMessage(level, std::format(std::move(message), std::forward<ArgsTy>(args)...));
            }
#endif
        }

#ifndef AME_DIST
        AME_LOG_TYPE(Trace);
        AME_LOG_TYPE(Debug);
        AME_LOG_TYPE(Info);
#else
        void Trace(...)
        {
        }
        void Debug(...)
        {
        }
        void Info(...)
        {
        }
#endif
        AME_LOG_TYPE(Warning);
        AME_LOG_TYPE(Error);
        AME_LOG_TYPE(Fatal);

        template<typename... ArgsTy>
        void Assert(bool condition, const std::format_string<ArgsTy...> message, ArgsTy&&... args) const
        {
#ifndef AME_DISABLE_LOGGING
#ifndef AME_DIST
            if (!condition)
            {
                Fatal(std::move(message), std::forward<ArgsTy>(args)...);
                Ame::DebugBreak();
            }
#endif
#endif
        }

        template<typename... ArgsTy>
        void Validate(bool condition, const std::format_string<ArgsTy...> message, ArgsTy&&... args) const
        {
#ifndef AME_DISABLE_LOGGING
            if (!condition)
            {
                Fatal(std::move(message), std::forward<ArgsTy>(args)...);
                std::unreachable();
            }
#endif
        }

    public:
        /// <summary>
        /// Set the current log level
        /// </summary>
        void SetLevel(LogLevel level);

        /// <summary>
        /// Get the current log level
        /// </summary>
        [[nodiscard]] LogLevel GetLevel() const noexcept;

    private:
        SharedPtr<spdlog::logger> m_Logger;
    };

    //

    [[nodiscard]] String FormatException(const std::exception& ex);
} // namespace Ame::Log
