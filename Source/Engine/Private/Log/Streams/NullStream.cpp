#include <Log/Streams/NullStream.hpp>
#include <spdlog/sinks/null_sink.h>

namespace Ame::Log
{
    NullStream::NullStream()
    {
        m_Sink = std::make_shared<spdlog::sinks::null_sink_mt>();
    }
} // namespace Ame::Log
