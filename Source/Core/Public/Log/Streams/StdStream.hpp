#pragma once

#include <Log/Stream.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/ostream_sink.h>

namespace Ame::Log
{
    class StdStream final : public ILoggerStream
    {
    public:
        StdStream(std::ostream& os, bool forceFlush = false)
        {
            m_Sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(os, forceFlush);
        }
    };
} // namespace Ame::Log
