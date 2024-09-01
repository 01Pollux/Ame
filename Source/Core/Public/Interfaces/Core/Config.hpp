#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

namespace Ame::Interfaces
{
    class FrameEvent;
    class FrameTimer;

    struct CoreModuleConfig
    {
        bool EnableFrameTimer : 1 = true;
        bool EnableFrameEvent : 1 = true;
        bool EnableLogger     : 1 = true;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces