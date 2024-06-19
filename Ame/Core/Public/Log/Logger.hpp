#pragma once

#include <Log/Signals.hpp>
#include <vector>

namespace spdlog
{
    namespace sinks
    {
        class sink;
    } // namespace sinks
    class logger;
} // namespace spdlog

#define AME_LOG_TYPE(Type)                                                      \
    template<typename... ArgsTy>                                                \
    void Type(                                                                  \
        const std::format_string<ArgsTy...> message,                            \
        ArgsTy&&... args) const                                                 \
    {                                                                           \
        Log(LogLevel::Type, std::move(message), std::forward<ArgsTy>(args)...); \
    }                                                                           \
    void Type(                                                                  \
        StringView message)                                                     \
    {                                                                           \
        LogMessage(LogLevel::Type, message);                                    \
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
        using SinkList = std::vector<Ptr<spdlog::sinks::sink>>;

    private:
        Logger()
        {
        }

        Logger(
            StringView tagName,
            StringView fileName,
            SinkList   sinks);

    public:
        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(
            const String& tagName,
            StringView    fileName,
            SinkList      sinks);

        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(
            const String& tagName,
            StringView    fileName);

        /// <summary>
        /// Register a null logger
        /// </summary>
        static void RegisterNull(
            const String& tagName);

        /// <summary>
        /// Unregister a logger
        /// </summary>
        static void Unregister(
            const String& tagName);

        /// <summary>
        /// Get global logger
        /// </summary>
        [[nodiscard]] static Logger& GetLogger(
            const String& name);

        /// <summary>
        /// Close all loggers
        /// </summary>
        static void CloseAllLoggers();

    public:
        /// <summary>
        /// Test if a log level can be logged
        /// </summary>
        [[nodiscard]] bool CanLog(
            LogLevel level) const noexcept;

    public:
        /// <summary>
        /// Log a message
        /// </summary>
        void LogMessage(
            LogLevel   level,
            StringView message) const;

        /// <summary>
        /// Log a message
        /// </summary>
        template<typename... ArgsTy>
        void Log(
            LogLevel                            level,
            const std::format_string<ArgsTy...> message,
            ArgsTy&&... args) const
        {
            if constexpr (sizeof...(ArgsTy) == 0)
            {
                LogMessage(level, message.get());
            }
            else
            {
                LogMessage(level, std::format(std::move(message), std::forward<ArgsTy>(args)...));
            }
        }

        AME_LOG_TYPE(Trace);
        AME_LOG_TYPE(Debug);
        AME_LOG_TYPE(Info);
        AME_LOG_TYPE(Warning);
        AME_LOG_TYPE(Error);
        AME_LOG_TYPE(Fatal);

        template<typename... ArgsTy>
        void Assert(
            bool                                condition,
            const std::format_string<ArgsTy...> message,
            ArgsTy&&... args) const
        {
#ifndef AME_DIST
            if (!condition)
            {
                Fatal(std::move(message), std::forward<ArgsTy>(args)...);
                Ame::DebugBreak();
            }
#endif
        }

        template<typename... ArgsTy>
        void Validate(
            bool                                condition,
            const std::format_string<ArgsTy...> message,
            ArgsTy&&... args) const
        {
            if (!condition)
            {
                Fatal(std::move(message), std::forward<ArgsTy>(args)...);
                std::unreachable();
            }
        }

    public:
        /// <summary>
        /// Set the current log level
        /// </summary>
        void SetLevel(
            LogLevel level);

        /// <summary>
        /// Get the current log level
        /// </summary>
        [[nodiscard]] LogLevel GetLevel() const noexcept;

    public:
        AME_SIGNAL_STATIC(OnLog);

    private:
        String              m_Name;
        Ptr<spdlog::logger> m_Logger;
    };
} // namespace Ame::Log
