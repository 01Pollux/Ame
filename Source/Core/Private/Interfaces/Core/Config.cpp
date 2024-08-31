#include <Interfaces/Core/FrameTimer.hpp>
#include <Interfaces/Core/FrameEvent.hpp>

namespace Ame::Interfaces
{
    void CoreModuleConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
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