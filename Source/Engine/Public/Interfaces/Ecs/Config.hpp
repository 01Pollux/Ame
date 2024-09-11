#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

namespace Ame::Interfaces
{
    struct EntityWorldCreateDesc
    {
        // Monitors are disabled on release builds
        bool EnableMonitors : 1 = true;
        bool Enable3DModule : 1 = true;
    };

    struct EcsModuleConfig
    {
        EntityWorldCreateDesc WorldDesc;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces
