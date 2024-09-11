#pragma once

#include <vector>
#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>
#include <Log/Stream.hpp>

namespace Ame::Interfaces
{
    class FrameEvent;
    class FrameTimer;

    struct AME_ENGINE_API LoggerCreateDesc
    {
        String                                     LoggerName = "Ame";
        std::vector<UniquePtr<Log::ILoggerStream>> Streams;
#ifdef AME_DEBUG
        Log::LogLevel DefaultLevel = Log::LogLevel::Trace;
#elif defined(AME_DIST)
        Log::LogLevel DefaultLevel = Log::LogLevel::Error;
#else
        Log::LogLevel DefaultLevel = Log::LogLevel::Info;
#endif
        bool SetAsMain = true;

        LoggerCreateDesc() = default;

        LoggerCreateDesc(const LoggerCreateDesc&)            = delete;
        LoggerCreateDesc& operator=(const LoggerCreateDesc&) = delete;

        LoggerCreateDesc(LoggerCreateDesc&&) noexcept            = default;
        LoggerCreateDesc& operator=(LoggerCreateDesc&&) noexcept = default;

        ~LoggerCreateDesc();
    };

    struct AME_ENGINE_API CoreModuleConfig
    {
        LoggerCreateDesc LoggerDesc;
        bool             EnableFrameTimer : 1 = true;
        bool             EnableFrameEvent : 1 = true;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const;
    };
} // namespace Ame::Interfaces