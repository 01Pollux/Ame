#pragma once

#include <Log/Stream.hpp>
#include <spdlog/sinks/null_sink.h>

namespace Ame::Log
{
    class NullStream final : public ILoggerStream
    {
    public:
        NullStream()
        {
            m_Sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        }
    };
} // namespace Ame::Log
