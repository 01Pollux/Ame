#pragma once

#include <Module/Submodule.hpp>
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

namespace Ame
{
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