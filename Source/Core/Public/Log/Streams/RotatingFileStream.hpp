#pragma once

#include <Log/Stream.hpp>
#include <FileSystem/Utils.hpp>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Ame::Log
{
    class RotatingFileStream final : public ILoggerStream
    {
    public:
        struct FileEvent
        {
            std::move_only_function<void(const StringView& fileName)>                        BeforeOpen;
            std::move_only_function<void(const StringView& fileName, std::FILE* fileStream)> AfterOpen;
            std::move_only_function<void(const StringView& fileName, std::FILE* fileStream)> BeforeClose;
            std::move_only_function<void(const StringView& fileName)>                        AfterClose;
        };

        RotatingFileStream(StringView baseFileName, size_t maxSize, size_t maxFiles, bool rotateOnOpen = false)
        {
            m_Sink =
                std::make_shared<spdlog::sinks::rotating_file_sink_mt>(baseFileName, maxSize, maxFiles, rotateOnOpen);
        }

        RotatingFileStream(StringView baseFileName, size_t maxSize, size_t maxFiles, bool rotateOnOpen = false,
                           FileEvent events = {})
        {
            spdlog::file_event_handlers handlers;

            handlers.before_open  = std::move(events.BeforeOpen);
            handlers.after_open   = std::move(events.AfterOpen);
            handlers.before_close = std::move(events.BeforeClose);
            handlers.after_close  = std::move(events.AfterClose);

            m_Sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                baseFileName, maxSize, maxFiles, rotateOnOpen, std::move(handlers));
        }
    };
} // namespace Ame::Log
