#pragma once

#include <Interfaces/Core/Config.hpp>
#include <Core/FrameTimer.hpp>

namespace Ame::Interfaces
{
    class FrameTimer final : public BaseObject<IObject>, public Ame::FrameTimer
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_FrameTimer, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(FrameTimer) :
            Base(counters)
        {
        }
    };
} // namespace Ame::Interfaces