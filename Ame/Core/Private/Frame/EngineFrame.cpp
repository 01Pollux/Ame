#include <Frame/EngineFrame.hpp>

namespace Ame
{
    EngineFrame::EngineFrame(
        FrameTimer& Timer) :
        m_Timer(Timer)
    {
    }

    void EngineFrame::Run()
    {
        while (m_IsRunning)
        {
            m_Timer.get().Tick();
            DoTick();
        }
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

    //

    void EngineFrame::DoTick()
    {
        OnStartFrame().Broadcast();

        if (!IsRunning())
        {
            return;
        }

        OnUpdate().Broadcast();
        OnPostUpdate().Broadcast();

        OnRender().Broadcast();
        OnPostRender().Broadcast();

        OnEndFrame().Broadcast();
    }
} // namespace Ame
