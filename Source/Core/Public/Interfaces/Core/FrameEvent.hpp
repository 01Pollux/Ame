#pragma once

#include <Interfaces/Core/Config.hpp>
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
        AME_SIGNAL_INST(OnFrameStart);
        AME_SIGNAL_INST(OnFrameUpdate);
        AME_SIGNAL_INST(OnFrameEnd);
    };
} // namespace Ame::Interfaces