#include <Log/Streams/CallbackStream.hpp>
#include <Log/Utils.hpp>
#include <spdlog/sinks/callback_sink.h>

namespace Ame::Log
{
    CallbackStream::CallbackStream()
    {
        auto callback = [this](const spdlog::details::log_msg& logMsg)
        {
            LogData logData{ StringView{ logMsg.payload.data(), logMsg.payload.size() }, logMsg.thread_id, logMsg.time,
                             GetLogLevel(logMsg.level) };
            OnLog(logData);
        };

        m_Sink = std::make_shared<spdlog::sinks::callback_sink_mt>(callback);
    }
} // namespace Ame::Log
