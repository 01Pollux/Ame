#include <Log/Streams/ConsoleStream.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Ame::Log
{
    ConsoleStream::ConsoleStream()
    {
        m_Sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }
} // namespace Ame::Log
