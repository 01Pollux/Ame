#pragma once

#include <Core/Interface.hpp>
#include <Core/Signal.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnFrameStart, void());
    AME_SIGNAL_DECL(OnFrameUpdate, void());
    /// <summary>
    /// Returns true if the application should continue running.
    /// </summary>
    AME_SIGNAL_DECL(OnFrameEnd, bool());
} // namespace Ame::Signals

namespace Ame::Interfaces
{
    // {919C726D-72E2-4573-ACC2-98EA5AF1781B}
    inline constexpr UId IID_FrameEvent{
        0x919c726d, 0x72e2, 0x4573, { 0xac, 0xc2, 0x98, 0xea, 0x5a, 0xf1, 0x78, 0x1b }
    };

    class FrameEvent final : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_FrameEvent, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(FrameEvent) : Base(counters)
        {
        }

    public:
        Signals::OnFrameStart_Signal  OnFrameStart;
        Signals::OnFrameUpdate_Signal OnFrameUpdate;
        Signals::OnFrameEnd_Signal    OnFrameEnd;
    };
} // namespace Ame::Interfaces