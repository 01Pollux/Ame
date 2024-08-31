#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

namespace Ame::Interfaces
{
    // {919C726D-72E2-4573-ACC2-98EA5AF1781B}
    inline constexpr UId IID_FrameEvent{ 0x919c726d, 0x72e2, 0x4573, { 0xac, 0xc2, 0x98, 0xea, 0x5a, 0xf1, 0x78, 0x1b } };
    class FrameEvent;

    // {BE4B917D-4B7A-4582-8034-01DC24147418}
    inline constexpr UId IID_FrameTimer{ 0xbe4b917d, 0x4b7a, 0x4582, { 0x80, 0x34, 0x1, 0xdc, 0x24, 0x14, 0x74, 0x18 } };
    class FrameTimer;

    struct CoreModuleConfig
    {
        bool EnableFrameTimer : 1 = true;
        bool EnableFrameEvent : 1 = true;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces