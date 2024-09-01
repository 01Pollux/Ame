#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    struct LogFileEvent;
    class RotatingFileStream final : public ILoggerStream
    {
    public:
        RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, bool rotateOnOpen = false);
        RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, LogFileEvent events,
                           bool rotateOnOpen = false);
    };
} // namespace Ame::Log
