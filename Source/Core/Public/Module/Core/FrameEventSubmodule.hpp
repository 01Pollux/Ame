#pragma once

#include <Module/Submodule.hpp>
#include <Core/Signal.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnFrameStart, void());
    AME_SIGNAL_DECL(OnFrameUpdate, void());
    AME_SIGNAL_DECL(OnFrameEnd, void());
} // namespace Ame::Signals

namespace Ame
{
    // {919C726D-72E2-4573-ACC2-98EA5AF1781B}
    static const UId IID_FrameEventSubmodule = { 0x919c726d, 0x72e2, 0x4573, { 0xac, 0xc2, 0x98, 0xea, 0x5a, 0xf1, 0x78, 0x1b } };

    class FrameEventSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_FrameEventSubmodule, IID_BaseSubmodule, Base);

    public:
        explicit FrameEventSubmodule(
            IReferenceCounters* counters) :
            Base(counters, IID_FrameEventSubmodule)
        {
        }

    public:
        AME_SIGNAL_INST(OnFrameStart);
        AME_SIGNAL_INST(OnFrameUpdate);
        AME_SIGNAL_INST(OnFrameEnd);
    };
} // namespace Ame