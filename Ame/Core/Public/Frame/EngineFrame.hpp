#pragma once

#include <Core/Signals/Frame.hpp>
#include <Frame/FrameTimer.hpp>

namespace Ame
{
    class EngineFrame
    {
    public:
        EngineFrame(
            FrameTimer& frameTimer);

        void Run();

    public:
        AME_SIGNAL_INST(OnStartFrame);

        AME_SIGNAL_INST(OnUpdate);
        AME_SIGNAL_INST(OnPostUpdate);

        AME_SIGNAL_INST(OnRender);
        AME_SIGNAL_INST(OnPostRender);

        AME_SIGNAL_INST(OnEndFrame);

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