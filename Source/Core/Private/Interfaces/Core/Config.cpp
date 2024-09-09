#include <Interfaces/Core/Config.hpp>
#include <Interfaces/Core/Logger.hpp>
#include <Interfaces/Core/Coroutine.hpp>
#include <Interfaces/Core/FrameTimer.hpp>
#include <Interfaces/Core/FrameEvent.hpp>

namespace Ame::Interfaces
{
    void CoreModuleConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
        if (!LoggerDesc.LoggerName.empty())
        {
            auto logger = Log::CreateLogger(LoggerDesc.LoggerName);

            for (auto& stream : LoggerDesc.Streams)
            {
                logger->AddStream(stream.get());
            }
            logger->SetLevel(LoggerDesc.DefaultLevel);

            registry->ExposeInterface(owner, IID_Logger, logger);

            if (LoggerDesc.SetAsMain)
            {
                Log::s_Logger = logger;
            }
        }
        registry->ExposeInterface(owner, IID_Coroutine, s_Coroutine);
        if (EnableFrameTimer)
        {
            registry->ExposeInterface(owner, IID_FrameTimer, AmeCreateObject(FrameTimer));
        }
        if (EnableFrameEvent)
        {
            registry->ExposeInterface(owner, IID_FrameEvent, AmeCreateObject(FrameEvent));
        }
    }
} // namespace Ame::Interfaces