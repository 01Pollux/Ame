#pragma once

#include <Object/Signal.hpp>
#include <Frame/FrameTimer.hpp>

namespace Ame
{
    class EngineFrame
    {
    public:
        AME_SIGNAL_DECL(OnStartFrame, void());

        AME_SIGNAL_DECL(OnTick, void());
        AME_SIGNAL_DECL(OnPostTick, void());

        AME_SIGNAL_DECL(OnEndFrame, void());

    public:
        EngineFrame(
            FrameTimer& frameTimer);

        void Run();

    public:
        /// <summary>
        /// Stop the frame loop
        /// </summary>
        void Stop();

        /// <summary>
        /// Check if the frame loop is running
        /// </summary>
        [[nodiscard]] bool IsRunning() const;

    public:
        AME_SIGNAL_INST(OnStartFrame);

        AME_SIGNAL_INST(OnTick);
        AME_SIGNAL_INST(OnPostTick);

        AME_SIGNAL_INST(OnEndFrame);

    private:
        Ref<FrameTimer> m_Timer;
        bool            m_IsRunning = true;
    };
} // namespace Ame