#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

#include <Ecs/World.hpp>

namespace Ame
{
    // {3C5C3A5B-2147-44D9-9DDA-594F891EDA8E}
    static constexpr UId IID_EntityModule{ 0x3c5c3a5b, 0x2147, 0x44d9, { 0x9d, 0xda, 0x59, 0x4f, 0x89, 0x1e, 0xda, 0x8e } };
    class EntityModule;

    // {195A8AE5-5E17-4832-9C41-1B03332A14B2}
    static constexpr UId IID_EntityStorageSubmodule{ 0x195a8ae5, 0x5e17, 0x4832, { 0x9c, 0x41, 0x1b, 0x3, 0x33, 0x2a, 0x14, 0xb2 } };
    class EntityStorageSubmodule;

    struct EcsModuleConfig
    {
        // Monitors are disabled on release builds
        bool EnableMonitors : 1 = true;
        bool Enable3DModule : 1 = true;
    };
} // namespace Ame
