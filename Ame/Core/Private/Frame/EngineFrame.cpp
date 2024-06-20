#include <Frame/EngineFrame.hpp>

namespace Ame
{
    EngineFrame::EngineFrame(
        Co::runtime& runtime,
        FrameTimer&  frameTimer) :
        m_Timer(frameTimer),
        m_StartFrame(runtime.make_manual_executor()),
        m_TickFrame(runtime.make_manual_executor()),
        m_EndFrame(runtime.make_manual_executor())
    {
    }

    void EngineFrame::Tick()
    {
        m_Timer.get().Tick();

        Co::manual_executor_loop_until_empty(m_StartFrame);
        Co::manual_executor_loop_until_empty(m_TickFrame);
        Co::manual_executor_loop_until_empty(m_EndFrame);
    }

    //

    void EngineFrame::Stop()
    {
        m_IsRunning = false;
    }

    bool EngineFrame::IsRunning() const
    {
        return m_IsRunning;
    }
} // namespace Ame
