#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    class AME_ENGINE_API CallbackStream : public ILoggerStream
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

        CallbackStream();

        virtual void OnLog(const LogData& logData) = 0;
    };
} // namespace Ame::Log
