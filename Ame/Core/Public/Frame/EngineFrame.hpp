#pragma once

#include <Signals/Core/Frame.hpp>
#include <Frame/FrameTimer.hpp>
#include <Core/Coroutine.hpp>

namespace Ame
{
    class EngineFrame
    {
    public:
        EngineFrame(
            Co::runtime& runtime,
            FrameTimer&  frameTimer);

        void Tick();

    public:
        AME_COROUTINE_INST(manual_executor, StartFrameExecutor);
        AME_COROUTINE_INST(manual_executor, EndFrameExecutor);

    public:
        AME_SIGNAL_INST(OnStartFrame);

        AME_SIGNAL_INST(OnUpdate);
        AME_SIGNAL_INST(OnPostUpdate);

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
        Ref<FrameTimer> m_Timer;
        bool            m_IsRunning = true;
    };
} // namespace Ame