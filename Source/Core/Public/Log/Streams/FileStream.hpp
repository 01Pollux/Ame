#pragma once

#include <Log/Stream.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/basic_file_sink.h>

namespace Ame::Log
{
    class FileStream final : public ILoggerStream
    {
    public:
        FileStream(const String& fileName)
        {
            if (!FileSystem::EnsurePathIsForward(fileName))
            {
                throw std::runtime_error("Invalid path for file stream.");
            }
            FileSystem::EnsureDirectoryExists(fileName);
            m_Sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>();
        }
    };
} // namespace Ame::Log
