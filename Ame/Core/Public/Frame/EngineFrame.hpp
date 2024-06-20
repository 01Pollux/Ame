#pragma once

#include <Object/Signal.hpp>
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
        [[nodiscard]] const auto& GetStartFrameExecutor() const
        {
            return m_StartFrame;
        }
        [[nodiscard]] const auto& GetTickFrameExecutor() const
        {
            return m_TickFrame;
        }
        [[nodiscard]] const auto GetEndFrameExecutor() const
        {
            return m_EndFrame;
        }

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

        Ptr<Co::manual_executor> m_StartFrame;
        Ptr<Co::manual_executor> m_TickFrame;
        Ptr<Co::manual_executor> m_EndFrame;
    };
} // namespace Ame