#pragma once

#include <Log/Stream.hpp>
#include <Log/Streams/LogFileEvent.hpp>

namespace Ame::Log
{
    class AME_ENGINE_API FileStream final : public ILoggerStream
    {
    public:
        FileStream(const String& fileName, bool truncate = false);
        FileStream(const String& fileName, LogFileEvent events, bool truncate = false);
    };
} // namespace Ame::Log
