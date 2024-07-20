#pragma once

#include <Module/Submodule.hpp>
#include <Core/FrameTimer.hpp>

namespace Ame
{
    // {BE4B917D-4B7A-4582-8034-01DC24147418}
    static const UId IID_TimeSubmodule = { 0xbe4b917d, 0x4b7a, 0x4582, { 0x80, 0x34, 0x1, 0xdc, 0x24, 0x14, 0x74, 0x18 } };

    class TimeSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_TimeSubmodule, IID_BaseSubmodule, Base);

    public:
        explicit TimeSubmodule(
            IReferenceCounters* counters) :
            Base(counters, IID_TimeSubmodule)
        {
        }

        [[nodiscard]] const FrameTimer& GetTimer() const noexcept
        {
            return m_Timer;
        }

        [[nodiscard]] FrameTimer& GetTimer() noexcept
        {
            return m_Timer;
        }

    private:
        FrameTimer m_Timer;
    };
} // namespace Ame