#pragma once

#include <Log/Core.hpp>
#include <Core/String.hpp>
#include <vector>

namespace spdlog
{
    namespace sinks
    {
        class sink;
    } // namespace sinks
    class logger;
} // namespace spdlog

#define AME_LOG_TYPE(Type)                                           \
    template<typename... ArgsTy>                                     \
    void Type(                                                       \
        StringU8View Message,                                        \
        ArgsTy&&... Args) const                                      \
    {                                                                \
        Log(LogLevel::Type, Message, std::forward<ArgsTy>(Args)...); \
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
    private:
        Logger()
        {
        }

        Logger(
            const StringU8&                       TagName,
            StringU8View                          FileName,
            std::vector<Ptr<spdlog::sinks::sink>> Sinks);

    public:
        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(
            const StringU8&                       TagName,
            StringU8View                          FileName,
            std::vector<Ptr<spdlog::sinks::sink>> Sinks);

        /// <summary>
        /// Register a logger
        /// </summary>
        static void Register(
            const StringU8& TagName,
            StringU8View    FileName);

        /// <summary>
        /// Register a null logger
        /// </summary>
        static void RegisterNull(
            const StringU8& TagName);

        /// <summary>
        /// Unregister a logger
        /// </summary>
        static void Unregister(
            const StringU8& TagName);

        /// <summary>
        /// Get global logger
        /// </summary>
        [[nodiscard]] static Logger& GetLogger(
            StringU8View Name);

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
            LogLevel     Level,
            StringU8View Message) const;

        /// <summary>
        /// Log a message
        /// </summary>
        template<typename... ArgsTy>
        void Log(
            LogLevel     Level,
            StringU8View Message,
            ArgsTy&&... Args) const
        {
            LogMessage(Level, StringUtils::Format(Message, std::forward<ArgsTy>(Args)...));
        }

        AME_LOG_TYPE(Trace);
        AME_LOG_TYPE(Debug);
        AME_LOG_TYPE(Info);
        AME_LOG_TYPE(Warning);
        AME_LOG_TYPE(Error);
        AME_LOG_TYPE(Fatal);

        template<typename... ArgsTy>
        void Assert(
            bool         Condition,
            StringU8View Message,
            ArgsTy&&... Args) const
        {
#ifndef AME_DIST
            if (!Condition)
            {
                Fatal(Message, std::forward<ArgsTy>(Args)...);
                AME_DEBUG_BREAK;
            }
#endif
        }

        template<typename... ArgsTy>
        void Validate(
            bool         Condition,
            StringU8View Message,
            ArgsTy&&... Args) const
        {
            if (!Condition)
            {
                Fatal(Message, std::forward<ArgsTy>(Args)...);
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

    private:
        StringU8            m_Name;
        Ptr<spdlog::logger> m_Logger;
    };
} // namespace Ame::Log
