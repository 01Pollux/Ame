#pragma once

#include <Log/Stream.hpp>

namespace Ame::Log
{
    struct LogFileEvent;
    class AME_ENGINE_API FileStream final : public ILoggerStream
    {
    public:
        FileStream(const String& fileName, bool truncate = false);
        FileStream(const String& fileName, LogFileEvent events, bool truncate = false);
    };
} // namespace Ame::Log
