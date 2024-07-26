#pragma once

#include <Core/Interface.hpp>

namespace Ame
{
    // {C9E70A4E-1AA4-4B93-B086-05314A078048}
    static constexpr UId IID_CoreModule = { 0xc9e70a4e, 0x1aa4, 0x4b93, { 0xb0, 0x86, 0x5, 0x31, 0x4a, 0x7, 0x80, 0x48 } };

    struct CoreModuleConfig
    {
        bool EnableTimeSubmodule       : 1 = true;
        bool EnableFrameEventSubmodule : 1 = true;
        bool EnableCoroutineSubmodule  : 1 = true;
    };

    class TimeSubmodule;
    class FrameEventSubmodule;
    class CoroutineSubmodule;
} // namespace Ame