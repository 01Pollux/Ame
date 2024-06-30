#include <Frame/EngineFrame.hpp>

namespace Ame
{
    EngineFrame::EngineFrame(
        Co::runtime& runtime) :
        m_StartFrameExecutor(runtime.make_manual_executor()),
        m_EndFrameExecutor(runtime.make_manual_executor())
    {
    }

    Co::result<void> EngineFrame::Tick(
        Co::runtime& runtime)
    {
        co_await Co::resume_on(runtime.inline_executor());

        m_OnStartFrame();

        Co::manual_executor_loop_until_empty(m_StartFrameExecutor);

        m_OnUpdate();
        m_OnPostUpdate();

        Co::manual_executor_loop_until_empty(m_EndFrameExecutor);

        m_OnEndFrame();
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
