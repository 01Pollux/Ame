#pragma once

#include <Core/Interface.hpp>
#include <Core/FrameTimer.hpp>

namespace Ame::Interfaces
{
    // {BE4B917D-4B7A-4582-8034-01DC24147418}
    inline constexpr UId IID_FrameTimer{
        0xbe4b917d, 0x4b7a, 0x4582, { 0x80, 0x34, 0x1, 0xdc, 0x24, 0x14, 0x74, 0x18 }
    };

    class AME_ENGINE_API FrameTimer final : public IObjectWithCallback, public Ame::FrameTimer
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_FrameTimer, IObjectWithCallback);

    private:
        IMPLEMENT_INTERFACE_CTOR(FrameTimer) : IObjectWithCallback(counters)
        {
        }
    };
} // namespace Ame::Interfaces