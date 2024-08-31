#pragma once

#include <Log/Stream.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/callback_sink.h>

namespace Ame::Log
{
    class CallbackStream final : public ILoggerStream
    {
    public:
        using LogTimePoint = std::chrono::system_clock::time_point;

        struct LogData
        {
            StringView   Message;
            size_t       ThreadId;
            LogTimePoint Timepoint;
            LogLevel     Level;
        };

        CallbackStream()
        {
            m_Sink = std::make_shared<spdlog::sinks::callback_sink_mt>([this](auto& logMsg) { LogCallback(logMsg); });
        }

        virtual void OnLog(const LogData& logData) = 0;

    private:
        void LogCallback(const spdlog::details::log_msg& logMsg);
    };
} // namespace Ame::Log
