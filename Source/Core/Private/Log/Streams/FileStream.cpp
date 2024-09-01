#include <Log/Streams/FileStream.hpp>
#include <Log/Utils.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/basic_file_sink.h>

namespace Ame::Log
{
    FileStream::FileStream(const String& fileName, bool truncate)
    {
        if (!FileSystem::EnsurePathIsForward(fileName))
        {
            throw std::runtime_error("Invalid path for file stream.");
        }
        FileSystem::EnsureDirectoryExists(fileName);
        m_Sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, truncate);
    }

    FileStream::FileStream(const String& fileName, LogFileEvent events, bool truncate)
    {
        if (!FileSystem::EnsurePathIsForward(fileName))
        {
            throw std::runtime_error("Invalid path for file stream.");
        }
        FileSystem::EnsureDirectoryExists(fileName);
        auto handlers = GetSpdlogFileEventHandlers(std::move(events));
        m_Sink        = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, truncate, handlers);
    }
} // namespace Ame::Log
