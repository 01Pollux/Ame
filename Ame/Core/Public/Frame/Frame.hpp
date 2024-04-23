#pragma once

#include <Core/Signals/Frame.hpp>
#include <Frame/Timer.hpp>

namespace Ame
{
    class IFrame
    {
    public:
        IFrame(
            FrameTimer& Timer);

        ~IFrame() = default;

        void Run();

    public:
        AME_SIGNAL_DOUBLE(OnStartFrame);

        AME_SIGNAL_DOUBLE(OnUpdate);
        AME_SIGNAL_DOUBLE(OnPostUpdate);

        AME_SIGNAL_DOUBLE(OnRender);
        AME_SIGNAL_DOUBLE(OnPostRender);

        AME_SIGNAL_DOUBLE(OnEndFrame);

    public:
        /// <summary>
        /// Stop the frame loop
        /// </summary>
        void Stop();

        /// <summary>
        /// Check if the frame loop is running
        /// </summary>
        [[nodiscard]] bool IsRunning() const;

    protected:
        virtual void DoTick() = 0;

    private:
        Ref<FrameTimer> m_Timer;
        bool            m_IsRunning = true;
    };
} // namespace Ame