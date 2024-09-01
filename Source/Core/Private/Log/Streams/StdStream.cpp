#include <Log/Streams/StdStream.hpp>
#include <spdlog/sinks/ostream_sink.h>

namespace Ame::Log
{
    StdStream::StdStream(std::ostream& os, bool forceFlush)
    {
        m_Sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(os, forceFlush);
    }
} // namespace Ame::Log
