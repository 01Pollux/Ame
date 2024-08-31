#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

namespace Ame::Interfaces
{
    // {195A8AE5-5E17-4832-9C41-1B03332A14B2}
    inline constexpr UId IID_EntityStorage{
        0x195a8ae5, 0x5e17, 0x4832, { 0x9c, 0x41, 0x1b, 0x3, 0x33, 0x2a, 0x14, 0xb2 }
    };
    class EntityStorage;

    struct EntityStorageCreateDesc
    {
        // Monitors are disabled on release builds
        bool EnableMonitors : 1 = true;
        bool Enable3DModule : 1 = true;
    };

    struct EcsModuleConfig
    {
        EntityStorageCreateDesc StorageDesc;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces
