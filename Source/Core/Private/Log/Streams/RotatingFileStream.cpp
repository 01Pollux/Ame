#include <Log/Streams/RotatingFileStream.hpp>
#include <Log/Utils.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/rotating_file_sink.h>

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

    RotatingFileStream::RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, bool rotateOnOpen)
    {
        ValidateFileAndInitDirectory(baseFileName);
        m_Sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            std::move(baseFileName), maxSize, maxFiles, rotateOnOpen);
    }

    RotatingFileStream::RotatingFileStream(String baseFileName, size_t maxSize, size_t maxFiles, LogFileEvent events,
                                           bool rotateOnOpen)
    {
        ValidateFileAndInitDirectory(baseFileName);
        auto handlers = GetSpdlogFileEventHandlers(std::move(events));
        m_Sink        = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            std::move(baseFileName), maxSize, maxFiles, rotateOnOpen, std::move(handlers));
    }
} // namespace Ame::Log
