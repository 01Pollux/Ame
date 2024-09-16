#pragma once

#include <Log/Stream.hpp>
#include <Log/Streams/LogFileEvent.hpp>

namespace Ame::Log
{
    class AME_ENGINE_API RotatingFileStream final : public ILoggerStream
    {
    public:
        RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, bool rotateOnOpen = false);
        RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, LogFileEvent events,
                           bool rotateOnOpen = false);
    };
} // namespace Ame::Log
