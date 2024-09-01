#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>
#include <Log/Stream.hpp>

namespace Ame::Interfaces
{
    class FrameEvent;
    class FrameTimer;

    struct LoggerCreateDesc
    {
        String                                     LoggerName = "Ame";
        std::vector<UniquePtr<Log::ILoggerStream>> Streams;
    };

    struct CoreModuleConfig
    {
        LoggerCreateDesc LoggerDesc;
        bool             EnableFrameTimer : 1 = true;
        bool             EnableFrameEvent : 1 = true;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces