#include <Log/Streams/FileStream.hpp>
#include <Log/Utils.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/basic_file_sink.h>

namespace Ame::Log
{
    static void ValidateFileAndInitDirectory(const String& fileName)
    {
        std::filesystem::path path(fileName);
        if (!FileSystem::EnsurePathIsForward(path))
        {
            throw std::runtime_error("Invalid path for file stream.");
        }
        if (path.has_root_directory())
        {
            FileSystem::EnsureDirectoryExists(path.root_directory());
        }
    }

    FileStream::FileStream(const String& fileName, bool truncate)
    {
        ValidateFileAndInitDirectory(fileName);

        m_Sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, truncate);
    }

    FileStream::FileStream(const String& fileName, LogFileEvent events, bool truncate)
    {
        ValidateFileAndInitDirectory(fileName);

        FileSystem::EnsureDirectoryExists(fileName);
        auto handlers = GetSpdlogFileEventHandlers(std::move(events));
        m_Sink        = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, truncate, handlers);
    }
} // namespace Ame::Log
