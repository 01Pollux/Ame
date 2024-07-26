#pragma once

#include <Core/Interface.hpp>

namespace Ame
{
    // {C9E70A4E-1AA4-4B93-B086-05314A078048}
    static constexpr UId IID_CoreModule = { 0xc9e70a4e, 0x1aa4, 0x4b93, { 0xb0, 0x86, 0x5, 0x31, 0x4a, 0x7, 0x80, 0x48 } };
    class CoreModule;

    // {8390DBB1-17D2-49BD-9C90-C70524E3E17A}
    static constexpr UId IID_CoroutineSubmodule = { 0x8390dbb1, 0x17d2, 0x49bd, { 0x9c, 0x90, 0xc7, 0x5, 0x24, 0xe3, 0xe1, 0x7a } };
    class CoroutineSubmodule;

    // {919C726D-72E2-4573-ACC2-98EA5AF1781B}
    static constexpr UId IID_FrameEventSubmodule = { 0x919c726d, 0x72e2, 0x4573, { 0xac, 0xc2, 0x98, 0xea, 0x5a, 0xf1, 0x78, 0x1b } };
    class FrameEventSubmodule;

    // {BE4B917D-4B7A-4582-8034-01DC24147418}
    static constexpr UId IID_TimeSubmodule = { 0xbe4b917d, 0x4b7a, 0x4582, { 0x80, 0x34, 0x1, 0xdc, 0x24, 0x14, 0x74, 0x18 } };
    class TimeSubmodule;

    struct CoreModuleConfig
    {
        bool EnableTimeSubmodule       : 1 = true;
        bool EnableFrameEventSubmodule : 1 = true;
        bool EnableCoroutineSubmodule  : 1 = true;
    };
} // namespace Ame