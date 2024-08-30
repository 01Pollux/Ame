#pragma once

#include <Module/Submodule.hpp>
#include <Module/Core/Config.hpp>
#include <Core/FrameTimer.hpp>

namespace Ame
{
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