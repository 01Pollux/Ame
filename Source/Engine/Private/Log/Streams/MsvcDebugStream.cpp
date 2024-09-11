#include <Log/Streams/MsvcDebugStream.hpp>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/null_sink.h>

namespace Ame::Log
{
    MsvcDebugStream::MsvcDebugStream()
    {
#if !defined(AME_DIST) && defined(AME_PLATFORM_WINDOWS)
        m_Sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
        m_Sink = std::make_shared<spdlog::sinks::null_sink_mt>();
#endif
    }
} // namespace Ame::Log
