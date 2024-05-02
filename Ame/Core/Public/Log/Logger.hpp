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
        const std::format_string<ArgsTy...> Message,                            \
        ArgsTy&&... Args) const                                                 \
    {                                                                           \
        Log(LogLevel::Type, std::move(Message), std::forward<ArgsTy>(Args)...); \
    }                                                                           \
    void Type(                                                                  \
        StringView Message)                                                     \
    {                                                                           \
        LogMessage(LogLevel::Type, Message);                                    \
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
            StringView TagName,
            StringView FileName,
            SinkList   Sinks);

    public:
        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(
            const String& TagName,
            StringView    FileName,
            SinkList      Sinks);

        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(
            const String& TagName,
            StringView    FileName);

        /// <summary>
        /// Register a null logger
        /// </summary>
        static void RegisterNull(
            const String& TagName);

        /// <summary>
        /// Unregister a logger
        /// </summary>
        static void Unregister(
            const String& TagName);

        /// <summary>
        /// Get global logger
        /// </summary>
        [[nodiscard]] static Logger& GetLogger(
            const String& Name);

        /// <summary>
        /// Close all loggers
        /// </summary>
        static void CloseAllLoggers();

    public:
        /// <summary>
        /// Test if a log level can be logged
        /// </summary>
        [[nodiscard]] bool CanLog(
            LogLevel Level) const noexcept;

    public:
        /// <summary>
        /// Log a message
        /// </summary>
        void LogMessage(
            LogLevel   Level,
            StringView Message) const;

        /// <summary>
        /// Log a message
        /// </summary>
        template<typename... ArgsTy>
        void Log(
            LogLevel                            Level,
            const std::format_string<ArgsTy...> Message,
            ArgsTy&&... Args) const
        {
            if constexpr (sizeof...(ArgsTy) == 0)
            {
                LogMessage(Level, Message.get());
            }
            else
            {
                LogMessage(Level, std::format(std::move(Message), std::forward<ArgsTy>(Args)...));
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
            bool                                Condition,
            const std::format_string<ArgsTy...> Message,
            ArgsTy&&... Args) const
        {
#ifndef AME_DIST
            if (!Condition)
            {
                Fatal(std::move(Message), std::forward<ArgsTy>(Args)...);
                AME_DEBUG_BREAK;
            }
#endif
        }

        template<typename... ArgsTy>
        void Validate(
            bool                                Condition,
            const std::format_string<ArgsTy...> Message,
            ArgsTy&&... Args) const
        {
            if (!Condition)
            {
                Fatal(std::move(Message), std::forward<ArgsTy>(Args)...);
                std::unreachable();
            }
        }

    public:
        /// <summary>
        /// Set the current log level
        /// </summary>
        void SetLevel(
            LogLevel Level);

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
