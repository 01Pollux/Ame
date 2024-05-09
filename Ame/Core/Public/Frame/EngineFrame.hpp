#pragma once

#include <Core/Signals/Frame.hpp>
#include <Frame/FrameTimer.hpp>

namespace Ame
{
    class EngineFrame
    {
    public:
        EngineFrame(
            FrameTimer& Timer);

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

    private:
        void DoTick();

    private:
        Ref<FrameTimer> m_Timer;
        bool            m_IsRunning = true;
    };
} // namespace Ame