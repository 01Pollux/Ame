#pragma once

#include <Log/Stream.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Ame::Log
{
    class ConsoleStream final : public ILoggerStream
    {
    public:
        ConsoleStream()
        {
            m_Sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        }
    };
} // namespace Ame::Log
