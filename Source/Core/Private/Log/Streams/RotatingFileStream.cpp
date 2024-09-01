#include <Log/Streams/RotatingFileStream.hpp>
#include <Log/Utils.hpp>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Ame::Log
{
    RotatingFileStream::RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, bool rotateOnOpen)
    {
        m_Sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            std::move(baseFileName), maxSize, maxFiles, rotateOnOpen);
    }

    RotatingFileStream::RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, LogFileEvent events,
                                           bool rotateOnOpen)
    {
        auto handlers = GetSpdlogFileEventHandlers(std::move(events));
        m_Sink        = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            std::move(baseFileName), maxSize, maxFiles, rotateOnOpen, std::move(handlers));
    }
} // namespace Ame::Log
