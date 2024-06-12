#include <Frame/EngineFrame.hpp>

namespace Ame
{
    EngineFrame::EngineFrame(
        FrameTimer& frameTimer) :
        m_Timer(frameTimer)
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
        m_OnStartFrame();

        if (!IsRunning()) [[unlikely]]
        {
            return;
        }

        m_OnUpdate();
        m_OnPostUpdate();

        m_OnRender();
        m_OnPostRender();

        m_OnEndFrame();
    }
} // namespace Ame
